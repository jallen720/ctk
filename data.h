#pragma once

#include <cstdio>
#include <cstdlib>
#include "ctk/ctk.h"

namespace ctk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct token
{
    enum class type
    {
        TEXT,
        STRING,
        ARRAY_OPEN,
        ARRAY_CLOSE,
        OBJECT_OPEN,
        OBJECT_CLOSE,
    };
    type Type;
    char *Data;
    u32 Size;
};

struct data
{
    enum class type
    {
        STRING,
        ARRAY,
        OBJECT,
    };
    type Type;
    string Key;
    union
    {
        string Value;
        array<data> Children;
    };
};

////////////////////////////////////////////////////////////
/// Declarations
////////////////////////////////////////////////////////////
static void
ParseChildren(array<data> *Children, array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType);

static data
ParseData(array<token> *Tokens, u32 *ParseTokenIndex, b32 ParseKey);

////////////////////////////////////////////////////////////
/// Internal
////////////////////////////////////////////////////////////
#include "ctk/_data_debug.h"

static cstr
TokenTypeName(token::type Type)
{
    switch(Type)
    {
        case token::type::TEXT: return "TEXT";
        case token::type::STRING: return "STRING";
        case token::type::ARRAY_OPEN: return "ARRAY_OPEN";
        case token::type::ARRAY_CLOSE: return "ARRAY_CLOSE";
        case token::type::OBJECT_OPEN: return "OBJECT_OPEN";
        case token::type::OBJECT_CLOSE: return "OBJECT_CLOSE";
        default: CTK_FATAL("unknown token type")
    };
}

static void
SkipToEndOfScope(array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType)
{
    // SkipToEndOfScope() called on open token; cache then start on next token.
    token::type OpenTokenType = At(Tokens, (*TokenIndex)++)->Type;

    CTK_LOOP
    {
        token *Token = At(Tokens, *TokenIndex);
        if(Token->Type == OpenTokenType)
        {
            // SkipToEndOfScope() leaves token index on token after closing bracket, so don't increment further.
            SkipToEndOfScope(Tokens, TokenIndex, CloseTokenType);
        }
        else
        {
            // Skip all other tokens, including close token.
            (*TokenIndex)++;
            if(Token->Type == CloseTokenType)
            {
                // End of scope reached.
                break;
            }
        }
    }
}

static void
ParseChildren(array<data> *Children, array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType)
{
    // ParseChildren() is called on an array/object open token so increment TokenIndex to first token in array/object.
    u32 ChildTokenIndex = ++(*TokenIndex);

    // Count children.
    u32 ChildCount = 0;
    CTK_LOOP
    {
        token *ChildToken = At(Tokens, ChildTokenIndex);
        if(ChildToken->Type == CloseTokenType)
        {
            break;
        }

        // Token represents a child.
        ChildCount++;

        // If object, skip key token.
        if(CloseTokenType == token::type::OBJECT_CLOSE)
        {
            ChildToken = At(Tokens, ++ChildTokenIndex);
        }

        // Skip to next child.
        switch(ChildToken->Type)
        {
            case token::type::TEXT:
            case token::type::STRING:
            {
                ChildTokenIndex++;
                break;
            }
            case token::type::ARRAY_OPEN:
            {
                SkipToEndOfScope(Tokens, &ChildTokenIndex, token::type::ARRAY_CLOSE);
                break;
            }
            case token::type::OBJECT_OPEN:
            {
                SkipToEndOfScope(Tokens, &ChildTokenIndex, token::type::OBJECT_CLOSE);
                break;
            }
        }
    }

    // Parse child data if any.
    if(ChildCount > 0)
    {
        *Children = CreateArrayEmpty<data>(ChildCount);
        while(At(Tokens, *TokenIndex)->Type != CloseTokenType)
        {
            // Only object children have keys.
            b32 ParseKey = CloseTokenType == token::type::OBJECT_CLOSE;

            Push(Children, ParseData(Tokens, TokenIndex, ParseKey));
            if(*TokenIndex >= Tokens->Count)
            {
                CTK_FATAL("reached end of file parsing children")
            }
        }
    }
}

static data
ParseData(array<token> *Tokens, u32 *ParseTokenIndex, b32 ParseKey)
{
    data Data = {};
    u32 TokenIndex = *ParseTokenIndex;

    // Parse key and move token index to next token which will represent the data's value.
    if(ParseKey)
    {
        token *KeyToken = At(Tokens, TokenIndex++);
        if(KeyToken->Type != token::type::TEXT)
        {
            CTK_FATAL("expected TEXT token for data key; found %s token instead", TokenTypeName(KeyToken->Type))
        }
        Data.Key = CreateString(KeyToken->Data, KeyToken->Size);
    }

    token *ValueToken = At(Tokens, TokenIndex);
    switch(ValueToken->Type)
    {
        case token::type::TEXT:
        {
            Data.Type = data::type::STRING;
            Data.Value = CreateString(ValueToken->Data, ValueToken->Size);
            break;
        }
        case token::type::STRING:
        {
            Data.Type = data::type::STRING;
            Data.Value = CreateString(ValueToken->Data + 1, ValueToken->Size - 2);
            break;
        }
        case token::type::ARRAY_OPEN:
        {
            Data.Type = data::type::ARRAY;
            ParseChildren(&Data.Children, Tokens, &TokenIndex, token::type::ARRAY_CLOSE);
            break;
        }
        case token::type::OBJECT_OPEN:
        {
            Data.Type = data::type::OBJECT;
            ParseChildren(&Data.Children, Tokens, &TokenIndex, token::type::OBJECT_CLOSE);
            break;
        }
    }
    *ParseTokenIndex = TokenIndex + 1;
    return Data;
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
static data
LoadData(cstr path)
{
    #define _CTK_CREATE_TOKEN(TYPE) \
        token *Token = Push(&Tokens); \
        Token->Type = token::type::TYPE; \
        Token->Data = DataString + DataCharIndex; \
        Token->Size = 1;

    // Wrap data from file in dummy "root" object to make it work better with parser.
    string DataFile = ReadTextFile(path);
    auto DataString = CreateStringEmpty(DataFile.Count + 7);
    Push(&DataString, "root{");
    Push(&DataString, &DataFile);
    Push(&DataString, "\n}");

    ////////////////////////////////////////////////////////////
    /// Token Parsing
    ////////////////////////////////////////////////////////////
    auto Tokens = CreateArrayEmpty<token>(DataString.Size / 2);
    for(u32 DataCharIndex = 0; DataCharIndex < DataString.Size; DataCharIndex++)
    {
        char DataChar = DataString[DataCharIndex];
        if(DataChar == ' ' ||
           DataChar == '\n' ||
           DataChar == '\r' ||
           DataChar == '\0')
        {
            continue;
        }
        else if(DataChar == '"')
        {
            _CTK_CREATE_TOKEN(STRING)

            // Expand cstr token to include all text.
            char *End = Token->Data + 1;
            while(*End != '"')
            {
                if(DataCharIndex + Token->Size == DataString.Size)
                {
                    CTK_FATAL("reached EOF trying to parse string")
                }
                Token->Size++;
                End++;
            }

            // Expand cstr token to include closing double-quotes.
            Token->Size++;

            // Set data character index to last character in token.
            DataCharIndex += Token->Size - 1;
        }
        else if(DataChar == '[')
        {
            _CTK_CREATE_TOKEN(ARRAY_OPEN)
        }
        else if(DataChar == ']')
        {
            _CTK_CREATE_TOKEN(ARRAY_CLOSE)
        }
        else if(DataChar == '{')
        {
            _CTK_CREATE_TOKEN(OBJECT_OPEN)
        }
        else if(DataChar == '}')
        {
            _CTK_CREATE_TOKEN(OBJECT_CLOSE)
        }
        else if(DataChar == '#')
        {
            // Skip rest of line for comments. It is not necessary to check for \0 as root scope adds \n} to end so
            // comment can never expand to end of string.
            while(DataString[++DataCharIndex] != '\n');
        }
        else
        {
            static cstr TEXT_TOKEN_END_CHARS = " \n\r[]{}\"";
            _CTK_CREATE_TOKEN(TEXT)

            // Expand text token to include all text up to any char in TEXT_TOKEN_END_CHARS.
            char *End = Token->Data + 1;
            b32 TextTokenEndReached = false;
            while(DataCharIndex + Token->Size != DataString.Size)
            {
                for(u32 TextTokenEndCharIndex = 0; TextTokenEndCharIndex < strlen(TEXT_TOKEN_END_CHARS); TextTokenEndCharIndex++)
                {
                    if(*End == TEXT_TOKEN_END_CHARS[TextTokenEndCharIndex])
                    {
                        TextTokenEndReached = true;
                        break;
                    }
                }
                if(TextTokenEndReached)
                {
                    break;
                }
                Token->Size++;
                End++;
            }

            // Set data character index to last character in token.
            DataCharIndex += Token->Size - 1;
        }
    }

    ////////////////////////////////////////////////////////////
    /// Data Parsing
    ////////////////////////////////////////////////////////////
    u32 TokenIndex = 0;
    data Data = ParseData(&Tokens, &TokenIndex, true);

    // Cleanup
    Free(&DataFile);
    Free(&DataString);
    Free(&Tokens);

    return Data;
}

static data *
At(data *Data, u32 Index)
{
    CTK_ASSERT(Index < Data->Children.Count)
    return Data->Children + Index;
}

static data *
At(data *Data, cstr Search)
{
    static const u32 MAX_KEY_SIZE = 64;
    char Key[MAX_KEY_SIZE + 1] = {}; // Ensure last char is null-terminator.
    const char *SearchEnd = Search + strlen(Search);
    enum class key_type { KEY, INDEX } KeyType = key_type::KEY;

    // If search is sub-search it will start on '.' if it's a key or '[' if it's an indexer.
    if(*Search == '.')
    {
        Search++;
    }
    else if(*Search == '[')
    {
        KeyType = key_type::INDEX;
        Search++;
    }

    u32 KeySize = 0;
    const char *CurrentChar = Search;
    CTK_LOOP
    {
        // If end of search is reached then search itself represents the key for the child to lookup.
        if(CurrentChar == SearchEnd)
        {
            break;
        }

        // End key cstr at '.' or '[' for keys or ']' for indexers.
        if(KeyType == key_type::KEY)
        {
            if(*CurrentChar == '.' || *CurrentChar == '[')
            {
                break;
            }
        }
        else
        {
            if(*CurrentChar == ']')
            {
                CurrentChar++;
                break;
            }
        }

        if(KeySize >= MAX_KEY_SIZE)
        {
            CTK_FATAL("key in search \"%s\" exceeds max key size of %u", Search, MAX_KEY_SIZE)
        }

        // Current char is valid part of key cstr; move to next char and increase key size.
        CurrentChar++;
        KeySize++;
    }

    // Write key and use it to find child, or fail if key doesn't match any child.
    memcpy(Key, Search, KeySize);
    data *Child = NULL;
    if(KeyType == key_type::KEY)
    {
        for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
        {
            data *CurrentChild = Data->Children + ChildIndex;
            if(StringEqual(&CurrentChild->Key, Key))
            {
                Child = CurrentChild;
                break;
            }
        }
    }
    else
    {
        Child = At(Data, StringToU32(Key));
    }

    if(!Child)
    {
        CTK_FATAL("failed to find child \"%s\"", Key)
    }

    // Return child if search has been completely processed, else search child starting at end of this search.
    return CurrentChar == SearchEnd ? Child : At(Child, CurrentChar);
}

template<typename ...args>
static data *
At(data *Data, cstr Search, args... Args)
{
    char FormattedSearch[128] = {};
    sprintf(FormattedSearch, Search, Args...);
    return At(Data, FormattedSearch);
}

template<typename ...args>
static string *
Value(data *Data, cstr Search, args... Args)
{
    return &At(Data, Search, Args...)->Value;
}

static string *
Value(data *Data, cstr Search)
{
    return &At(Data, Search)->Value;
}

static string *
Value(data *Data, u32 Index)
{
    return &At(Data, Index)->Value;
}

template<typename ...args>
static cstr
CStr(data *Data, cstr Search, args... Args)
{
    return Value(Data, Search, Args...)->Data;
}

static cstr
CStr(data *Data, cstr Search)
{
    return Value(Data, Search)->Data;
}

static cstr
CStr(data *Data, u32 Index)
{
    return Value(Data, Index)->Data;
}

template<typename ...args>
static f32
F32(data *Data, cstr Search, args... Args)
{
    return StringToF32(Value(Data, Search, Args...));
}

static f32
F32(data *Data, cstr Search)
{
    return StringToF32(Value(Data, Search));
}

static f32
F32(data *Data, u32 Index)
{
    return StringToF32(Value(Data, Index));
}

template<typename ...args>
static u32
U32(data *Data, cstr Search, args... Args)
{
    return StringToU32(Value(Data, Search, Args...));
}

static u32
U32(data *Data, cstr Search)
{
    return StringToU32(Value(Data, Search));
}

static u32
U32(data *Data, u32 Index)
{
    return StringToU32(Value(Data, Index));
}

template<typename ...args>
static s32
S32(data *Data, cstr Search, args... Args)
{
    return StringToS32(Value(Data, Search, Args...));
}

static s32
S32(data *Data, cstr Search)
{
    return StringToS32(Value(Data, Search));
}

static s32
S32(data *Data, u32 Index)
{
    return StringToS32(Value(Data, Index));
}

template<typename ...args>
static b32
B32(data *Data, cstr Search, args... Args)
{
    return StringToB32(Value(Data, Search, Args...));
}

static b32
B32(data *Data, cstr Search)
{
    return StringToB32(Value(Data, Search));
}

static b32
B32(data *Data, u32 Index)
{
    return StringToB32(Value(Data, Index));
}

static void
Free(data *Data)
{
    if(Data->Key.Count > 0)
    {
        Free(&Data->Key);
    }
    switch(Data->Type)
    {
        case data::type::STRING:
        {
            Free(&Data->Value);
            break;
        }
        case data::type::ARRAY:
        case data::type::OBJECT:
        {
            if(Data->Children.Count > 0)
            {
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    Free(Data->Children + ChildIndex);
                }
                Free(&Data->Children);
            }
            break;
        }
    }
}

} // ctk
