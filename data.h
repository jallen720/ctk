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
parse_children(array<data> *Children, array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType);

static data
parse_data(array<token> *Tokens, u32 *ParseTokenIndex, b32 ParseKey);

static data *
at(data *Data, cstr Search);

static data *
at(data *Data, u32 Index);

////////////////////////////////////////////////////////////
/// Internal
////////////////////////////////////////////////////////////
#include "ctk/_data_debug.h"

static cstr
token_type_name(token::type Type)
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
skip_to_end_of_scope(array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType)
{
    // skip_to_end_of_scope() called on open token; cache then start on next token.
    token::type OpenTokenType = at(Tokens, (*TokenIndex)++)->Type;

    while(1)
    {
        token *Token = at(Tokens, *TokenIndex);
        if(Token->Type == OpenTokenType)
        {
            // skip_to_end_of_scope() leaves token index on token after closing bracket, so don't increment further.
            skip_to_end_of_scope(Tokens, TokenIndex, CloseTokenType);
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
parse_children(array<data> *Children, array<token> *Tokens, u32 *TokenIndex, token::type CloseTokenType)
{
    // parse_children() is called on an array/object open token so increment TokenIndex to first token in array/object.
    u32 ChildTokenIndex = ++(*TokenIndex);

    // Count children.
    u32 ChildCount = 0;
    while(1)
    {
        token *ChildToken = at(Tokens, ChildTokenIndex);
        if(ChildToken->Type == CloseTokenType)
        {
            break;
        }

        // Token represents a child.
        ChildCount++;

        // If object, skip key token.
        if(CloseTokenType == token::type::OBJECT_CLOSE)
        {
            ChildToken = at(Tokens, ++ChildTokenIndex);
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
                skip_to_end_of_scope(Tokens, &ChildTokenIndex, token::type::ARRAY_CLOSE);
                break;
            }
            case token::type::OBJECT_OPEN:
            {
                skip_to_end_of_scope(Tokens, &ChildTokenIndex, token::type::OBJECT_CLOSE);
                break;
            }
        }
    }

    // Parse child data if any.
    if(ChildCount > 0)
    {
        *Children = create_array_empty<data>(ChildCount);
        while(at(Tokens, *TokenIndex)->Type != CloseTokenType)
        {
            // Only object children have keys.
            b32 ParseKey = CloseTokenType == token::type::OBJECT_CLOSE;

            push(Children, parse_data(Tokens, TokenIndex, ParseKey));
            if(*TokenIndex >= Tokens->Count)
            {
                CTK_FATAL("reached end of file parsing children")
            }
        }
    }
}

static data
parse_data(array<token> *Tokens, u32 *ParseTokenIndex, b32 ParseKey)
{
    data Data = {};
    u32 TokenIndex = *ParseTokenIndex;

    // Parse key and move token index to next token which will represent the data's value.
    if(ParseKey)
    {
        token *KeyToken = at(Tokens, TokenIndex++);
        if(KeyToken->Type != token::type::TEXT)
        {
            CTK_FATAL("expected TEXT token for data key; found %s token instead", token_type_name(KeyToken->Type))
        }
        Data.Key = create_string(KeyToken->Data, KeyToken->Size);
    }

    token *ValueToken = at(Tokens, TokenIndex);
    switch(ValueToken->Type)
    {
        case token::type::TEXT:
        {
            Data.Type = data::type::STRING;
            Data.Value = create_string(ValueToken->Data, ValueToken->Size);
            break;
        }
        case token::type::STRING:
        {
            Data.Type = data::type::STRING;
            Data.Value = create_string(ValueToken->Data + 1, ValueToken->Size - 2);
            break;
        }
        case token::type::ARRAY_OPEN:
        {
            Data.Type = data::type::ARRAY;
            parse_children(&Data.Children, Tokens, &TokenIndex, token::type::ARRAY_CLOSE);
            break;
        }
        case token::type::OBJECT_OPEN:
        {
            Data.Type = data::type::OBJECT;
            parse_children(&Data.Children, Tokens, &TokenIndex, token::type::OBJECT_CLOSE);
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
load_data(cstr path)
{
    #define _CTK_CREATE_TOKEN(TYPE) \
        token *Token = push(&Tokens); \
        Token->Type = token::type::TYPE; \
        Token->Data = DataString + DataCharIndex; \
        Token->Size = 1;

    // Wrap data from file in dummy "root" object to make it work better with parser.
    string DataFile = read_text_file(path);
    auto DataString = create_string_empty(DataFile.Count + 7);
    push(&DataString, "root{");
    push(&DataString, &DataFile);
    push(&DataString, "\n}");

    ////////////////////////////////////////////////////////////
    /// Token Parsing
    ////////////////////////////////////////////////////////////
    auto Tokens = create_array_empty<token>(DataString.Size / 2);
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
    data Data = parse_data(&Tokens, &TokenIndex, true);

    // Cleanup
    _free(&DataFile);
    _free(&DataString);
    _free(&Tokens);

    return Data;
}

static data *
find(data *Data, cstr Search)
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
    while(1)
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
            if(equal(&CurrentChild->Key, Key))
            {
                Child = CurrentChild;
                break;
            }
        }
    }
    else
    {
        Child = at(Data, to_u32(Key));
    }

    if(!Child)
    {
        return NULL;
    }

    // Return child if search has been completely processed, else search child starting at end of this search.
    return CurrentChar == SearchEnd ? Child : at(Child, CurrentChar);
}

static data *
at(data *Data, cstr Search)
{
    data *Child = find(Data, Search);
    if(!Child)
    {
        CTK_FATAL("failed to find child from search \"%s\"", Search)
    }
    return Child;
}

static data *
at(data *Data, u32 Index)
{
    CTK_ASSERT(Index < Data->Children.Count)
    return Data->Children + Index;
}

template<typename ...args>
static data *
at(data *Data, cstr Search, args... Args)
{
    char FormattedSearch[128] = {};
    sprintf(FormattedSearch, Search, Args...);
    return at(Data, FormattedSearch);
}

template<typename ...args>
static string *
value(data *Data, cstr Search, args... Args)
{
    return &at(Data, Search, Args...)->Value;
}

static string *
value(data *Data, cstr Search)
{
    return &at(Data, Search)->Value;
}

static string *
value(data *Data, u32 Index)
{
    return &at(Data, Index)->Value;
}

template<typename ...args>
static cstr
to_cstr(data *Data, cstr Search, args... Args)
{
    return value(Data, Search, Args...)->Data;
}

static cstr
to_cstr(data *Data, cstr Search)
{
    return value(Data, Search)->Data;
}

static cstr
to_cstr(data *Data, u32 Index)
{
    return value(Data, Index)->Data;
}

static cstr
to_cstr(data *Data)
{
    return Data->Value.Data;
}

template<typename ...args>
static f32
to_f32(data *Data, cstr Search, args... Args)
{
    return to_f32(value(Data, Search, Args...));
}

static f32
to_f32(data *Data, cstr Search)
{
    return to_f32(value(Data, Search));
}

static f32
to_f32(data *Data, u32 Index)
{
    return to_f32(value(Data, Index));
}

static f32
to_f32(data *Data)
{
    return to_f32(&Data->Value);
}

template<typename ...args>
static u32
to_u32(data *Data, cstr Search, args... Args)
{
    return to_u32(value(Data, Search, Args...));
}

static u32
to_u32(data *Data, cstr Search)
{
    return to_u32(value(Data, Search));
}

static u32
to_u32(data *Data, u32 Index)
{
    return to_u32(value(Data, Index));
}

static u32
to_u32(data *Data)
{
    return to_u32(&Data->Value);
}

template<typename ...args>
static s32
to_s32(data *Data, cstr Search, args... Args)
{
    return to_s32(value(Data, Search, Args...));
}

static s32
to_s32(data *Data, cstr Search)
{
    return to_s32(value(Data, Search));
}

static s32
to_s32(data *Data, u32 Index)
{
    return to_s32(value(Data, Index));
}

static s32
to_s32(data *Data)
{
    return to_s32(&Data->Value);
}

template<typename ...args>
static b32
to_b32(data *Data, cstr Search, args... Args)
{
    return to_b32(value(Data, Search, Args...));
}

static b32
to_b32(data *Data, cstr Search)
{
    return to_b32(value(Data, Search));
}

static b32
to_b32(data *Data, u32 Index)
{
    return to_b32(value(Data, Index));
}

static b32
to_b32(data *Data)
{
    return to_b32(&Data->Value);
}

static void
_free(data *Data)
{
    if(Data->Key.Count > 0)
    {
        _free(&Data->Key);
    }
    switch(Data->Type)
    {
        case data::type::STRING:
        {
            _free(&Data->Value);
            break;
        }
        case data::type::ARRAY:
        case data::type::OBJECT:
        {
            if(Data->Children.Count > 0)
            {
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    _free(Data->Children + ChildIndex);
                }
                _free(&Data->Children);
            }
            break;
        }
    }
}

} // ctk
