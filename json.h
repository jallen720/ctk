/// Data
////////////////////////////////////////////////////////////
struct JSONTokenParseState
{
    uint32 char_index;
    uint32 line;
    uint32 column;
};

enum struct JSONTokenType
{
    // Symbols
    OPEN_SQUARE_BRACKET,
    CLOSE_SQUARE_BRACKET,
    OPEN_CURLY_BRACKET,
    CLOSE_CURLY_BRACKET,
    COLON,
    COMMA,

    // Data Types
    UINT32,
    SINT32,
    FLOAT32,
    STRING,

    // Literals
    BOOLEAN,
    NULL_,

    NONE,
};

struct JSONToken
{
    JSONTokenType type;
    uint32        index;
    uint32        size;
};

struct JSONListState
{
    uint32 node_index;
    uint32 token_index;
};

struct JSONNodeParseState
{
    Array<JSONListState> list_states;
    uint32               key_index;
    uint32               key_offset;
    uint32               value_offset;
};

enum struct JSONNodeType
{
    ARRAY,
    OBJECT,
    UINT32,
    SINT32,
    FLOAT32,
    STRING,
    BOOLEAN,
    NULL_,
};

struct JSONNodeList
{
    uint32 node_index;
    uint32 key_index;
    uint32 size;
};

struct JSONNode
{
    uint32       key_index;
    String       key;
    JSONNodeType type;
    union
    {
        uint32       num_uint32;
        sint32       num_sint32;
        float32      num_float32;
        String       string;
        bool         boolean;
        JSONNodeList list;
    };
};

struct JSON
{
    Allocator*      allocator;
    JSONNode        root_node;
    Array<JSONNode> nodes;
    uint32          string_buffer_size;
    uint32          max_keys;
    char*           string_buffer;
    uint32*         key_sizes;
    uint32*         key_offsets;
};

using PrintChildFunc = Func<void, JSONNode*, uint32, uint32, uint32>;
struct PrintChildFuncs
{
    PrintChildFunc array;
    PrintChildFunc object;
};

constexpr bool JSON_SYMBOL[128] =
{
    /*0*/   false, false, false,     false,     false,     false,     false, false,
    /*8*/   false, false, false,     false,     false,     false,     false, false,
    /*16*/  false, false, false,     false,     false,     false,     false, false,
    /*24*/  false, false, false,     false,     false,     false,     false, false,
    /*32*/  false, false, false,     false,     false,     false,     false, false,
    /*40*/  false, false, false,     false,     true/*,*/, false,     false, false,
    /*48*/  false, false, false,     false,     false,     false,     false, false,
    /*56*/  false, false, true/*:*/, false,     false,     false,     false, false,
    /*64*/  false, false, false,     false,     false,     false,     false, false,
    /*72*/  false, false, false,     false,     false,     false,     false, false,
    /*80*/  false, false, false,     false,     false,     false,     false, false,
    /*88*/  false, false, false,     true/*[*/, false,     true/*]*/, false, false,
    /*96*/  false, false, false,     false,     false,     false,     false, false,
    /*104*/ false, false, false,     false,     false,     false,     false, false,
    /*112*/ false, false, false,     false,     false,     false,     false, false,
    /*120*/ false, false, false,     true/*{*/, false,     true/*}*/, false, false,
};

constexpr bool JSON_SKIPPABLE[128] =
{
    /*0*/   true/*\0*/, false,      false,      false, false, false,      false, false,
    /*8*/   false,      true/*\t*/, true/*\n*/, false, false, true/*\r*/, false, false,
    /*16*/  false,      false,      false,      false, false, false,      false, false,
    /*24*/  false,      false,      false,      false, false, false,      false, false,
    /*32*/  true/* */,  false,      false,      false, false, false,      false, false,
    /*40*/  false,      false,      false,      false, false, false,      false, false,
    /*48*/  false,      false,      false,      false, false, false,      false, false,
    /*56*/  false,      false,      false,      false, false, false,      false, false,
    /*64*/  false,      false,      false,      false, false, false,      false, false,
    /*72*/  false,      false,      false,      false, false, false,      false, false,
    /*80*/  false,      false,      false,      false, false, false,      false, false,
    /*88*/  false,      false,      false,      false, false, false,      false, false,
    /*96*/  false,      false,      false,      false, false, false,      false, false,
    /*104*/ false,      false,      false,      false, false, false,      false, false,
    /*112*/ false,      false,      false,      false, false, false,      false, false,
    /*120*/ false,      false,      false,      false, false, false,      false, false,
};

constexpr JSONTokenType JSON_TOKEN_TYPE_SYMBOL[128] =
{
    /*0*/   JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*8*/   JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*16*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*24*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*32*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*40*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::COMMA, JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*48*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*56*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::COLON, JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*64*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*72*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*80*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*88*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::OPEN_SQUARE_BRACKET, JSONTokenType::NONE,  JSONTokenType::CLOSE_SQUARE_BRACKET, JSONTokenType::NONE, JSONTokenType::NONE,
    /*96*/  JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*104*/ JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*112*/ JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::NONE,                JSONTokenType::NONE,  JSONTokenType::NONE,                 JSONTokenType::NONE, JSONTokenType::NONE,
    /*120*/ JSONTokenType::NONE, JSONTokenType::NONE, JSONTokenType::NONE,  JSONTokenType::OPEN_CURLY_BRACKET,  JSONTokenType::NONE,  JSONTokenType::CLOSE_CURLY_BRACKET,  JSONTokenType::NONE, JSONTokenType::NONE,
};

/// Utils
////////////////////////////////////////////////////////////
const char* TokenTypeName(JSONTokenType type)
{
    constexpr const char* TOKEN_TYPE_NAMES[] =
    {
        // Symbols
        "OPEN_SQUARE_BRACKET",
        "CLOSE_SQUARE_BRACKET",
        "OPEN_CURLY_BRACKET",
        "CLOSE_CURLY_BRACKET",
        "COLON",
        "COMMA",

        // Data Types
        "UINT32",
        "SINT32",
        "FLOAT32",
        "STRING",

        // Literals
        "BOOLEAN",
        "NULL",

        "NONE",
    };
    CTK_ASSERT((uint32)type < CTK_ARRAY_SIZE(TOKEN_TYPE_NAMES));
    return TOKEN_TYPE_NAMES[(uint32)type];
}

const char* NodeTypeName(JSONNodeType type)
{
    constexpr const char* NODE_TYPE_NAMES[] =
    {
        "ARRAY",
        "OBJECT",
        "UINT32",
        "SINT32",
        "FLOAT32",
        "STRING",
        "BOOLEAN",
        "NULL",
    };
    CTK_ASSERT((uint32)type < CTK_ARRAY_SIZE(NODE_TYPE_NAMES));
    return NODE_TYPE_NAMES[(uint32)type];
}

void ParseLiteralToken(JSONTokenParseState* parse_state, String* json_file, const char* value, uint32 size)
{
    if (parse_state->char_index + size >= json_file->count)
    {
        CTK_FATAL("reached end of JSON file parsing literal %s on line %u column %u", value,
                  parse_state->line,
                  parse_state->column);
    }
    if (!StringsMatch(&json_file->data[parse_state->char_index], value, size))
    {
        CTK_FATAL("invalid literal %s on line %u column %u", value,
                  parse_state->line,
                  parse_state->column);
    }
    parse_state->char_index += size;
    parse_state->column     += size;
}

Array<JSONToken> ParseTokens(JSON* json, String* json_file)
{
    Frame frame = CreateFrame();

    auto tokens = CreateArray<JSONToken>(json->allocator);

    // Parse File
    JSONTokenParseState parse_state =
    {
        .char_index = 0,
        .line       = 1,
        .column     = 1,
    };

    // Count max tokens.
    uint32 max_tokens = 0;
    uint32 depth      = 0;
    uint32 max_depth  = 0;
    while (parse_state.char_index < json_file->count)
    {
        char c = json_file->data[parse_state.char_index];
        if (c == '\"')
        {
            uint32 start_line = parse_state.line;
            uint32 start_column = parse_state.column;
            max_tokens += 1;
            parse_state.char_index += 1; // Skip opening quotation mark.
            parse_state.column += 1;

            // Find end of string token.
            bool escape_next = false;
            while (true)
            {
                if (parse_state.char_index >= json_file->count)
                {
                    CTK_FATAL("reached EOF while parsing string starting on line %u column %u",
                              start_line,
                              start_column);
                }
                char string_char = json_file->data[parse_state.char_index];
                if (escape_next)
                {
                    escape_next = false;
                }
                else if (string_char == '\\')
                {
                    escape_next = true;
                }
                else if (string_char == '\n')
                {
                    parse_state.line += 1;
                    parse_state.column = 0; // Reset to 0 so column increment after char_index increment will be 1.
                }
                else if (string_char == '\"')
                {
                    parse_state.char_index += 1; // Skips close quotation mark.
                    parse_state.column += 1;
                    break;
                }
                parse_state.char_index += 1;
                parse_state.column += 1;
                json->string_buffer_size += 1;
            }
        }
        else if (c == 'n')
        {
            max_tokens += 1;
            ParseLiteralToken(&parse_state, json_file, "null", 4);

            // Parsing ends after last char of value; no need to increment parse_state.char_index.
        }
        else if (c == 't')
        {
            max_tokens += 1;
            ParseLiteralToken(&parse_state, json_file, "true", 4);

            // Parsing ends after last char of value; no need to increment parse_state.char_index.
        }
        else if (c == 'f')
        {
            max_tokens += 1;
            ParseLiteralToken(&parse_state, json_file, "false", 5);

            // Parsing ends after last char of value; no need to increment parse_state.char_index.
        }
        else if (ASCII_NUMERIC[c] || c == '-')
        {
            max_tokens += 1;
            parse_state.char_index += 1;
            parse_state.column += 1;

            JSONTokenType token_type = JSONTokenType::UINT32;

            // Parse char after negative sign.
            if (c == '-')
            {
                if (parse_state.char_index >= json_file->count)
                {
                    CTK_FATAL("reached EOF while parsing number after negative sign on line %u column %u",
                              parse_state.line,
                              parse_state.column);
                }
                if (!ASCII_NUMERIC[json_file->data[parse_state.char_index]])
                {
                    CTK_FATAL("non-numeric character following negative sign on line %u column %u: '%c'",
                              parse_state.line,
                              parse_state.column,
                              json_file->data[parse_state.char_index]);
                }
                token_type = JSONTokenType::SINT32;
                parse_state.char_index += 1;
                parse_state.column += 1;
            }

            // Parse rest of numeric value.
            while (parse_state.char_index < json_file->count)
            {
                char num_char = json_file->data[parse_state.char_index];
                if (num_char == '.')
                {
                    if (token_type == JSONTokenType::FLOAT32)
                    {
                        CTK_FATAL("multiple decimals in numeric value on line %u column %u",
                                  parse_state.line,
                                  parse_state.column);
                    }
                    else
                    {
                        token_type = JSONTokenType::FLOAT32;
                    }
                }
                else if (num_char == 'e')
                {
                    parse_state.char_index += 1;
                    parse_state.column += 1;
                    num_char = json_file->data[parse_state.char_index];
                    if (num_char != '-' && num_char != '+' && !ASCII_NUMERIC[num_char])
                    {
                        CTK_FATAL("character following e on line %u column %u must be '-', '+' or a numeric value for "
                                  "the exponent",
                                  parse_state.line,
                                  parse_state.column)
                    }
                    if (num_char == '-' || num_char == '+')
                    {
                        parse_state.char_index += 1;
                        parse_state.column += 1;
                    }
                    while (parse_state.char_index < json_file->count)
                    {
                        num_char = json_file->data[parse_state.char_index];
                        if (!ASCII_NUMERIC[num_char])
                        {
                            break;
                        }
                        parse_state.char_index += 1;
                        parse_state.column += 1;
                    }
                    break;
                }
                else if (!ASCII_NUMERIC[num_char])
                {
                    break;
                }
                parse_state.char_index += 1;
                parse_state.column += 1;
            }

            // Parsing ends after last char of value; no need to increment parse_state.char_index.
        }
        else if (JSON_SKIPPABLE[c])
        {
            // Skip
            if (c == '\n')
            {
                parse_state.line += 1;
                parse_state.column = 0; // Reset to 0 so column increment after char_index increment will be 1.
            }
            parse_state.char_index += 1;
            parse_state.column += 1;
        }
        else if (JSON_SYMBOL[c])
        {
            max_tokens += 1;

            JSONTokenType token_type = JSON_TOKEN_TYPE_SYMBOL[c];
            if (token_type == JSONTokenType::NONE)
            {
                CTK_FATAL("failed to find symbol token type for symbol character on line %u column %u: \'%c\'",
                          parse_state.line,
                          parse_state.column,
                          c);
            }

            if (token_type == JSONTokenType::OPEN_SQUARE_BRACKET ||
                token_type == JSONTokenType::OPEN_CURLY_BRACKET)
            {
                depth += 1;
                if (depth > max_depth)
                {
                    max_depth = depth;
                }
            }
            else if (token_type == JSONTokenType::CLOSE_SQUARE_BRACKET ||
                     token_type == JSONTokenType::CLOSE_CURLY_BRACKET)
            {
                if (depth == 0)
                {
                    CTK_FATAL("found extraneous close bracket on line %u column %u: \'%c\'",
                              parse_state.line,
                              parse_state.column,
                              c);
                }
                depth -= 1;
            }

            // Every key has a colon following it.
            if (token_type == JSONTokenType::COLON)
            {
                json->max_keys += 1;
            }

            // Parsing ends on last char of token; skip to next.
            parse_state.char_index += 1;
            parse_state.column += 1;
        }
        else
        {
            CTK_FATAL("invalid character on line %u column %u: \'%c\'",
                      parse_state.line,
                      parse_state.column,
                      c);
        }
    }
    if (max_tokens == 0)
    {
        return tokens;
    }

    // Parse tokens.
    parse_state.char_index = 0;
    Resize(&tokens, max_tokens);

    // Track what list tokens belong to. An extra level is added to stack to allow resetting is_array flag on the final
    // close bracket. This removes the need to check is_array_stack.count > 0 every time is_array needs reset.
    auto is_array_stack = CreateArray<bool>(&frame, max_depth + 1);
    bool is_array = false;
    Push(&is_array_stack, false);

    while (parse_state.char_index < json_file->count)
    {
        char c = json_file->data[parse_state.char_index];
        if (c == '\"')
        {
            // Every string token will have its own node.
            if (is_array)
            {
                json->nodes.size += 1;
            }

            parse_state.char_index += 1; // Skip opening quotation mark.

            JSONToken* token = Push(&tokens);
            token->type  = JSONTokenType::STRING;
            token->index = parse_state.char_index;
            token->size  = 0;

            // Find end of string token.
            bool escape_next = false;
            while (true)
            {
                char string_char = json_file->data[parse_state.char_index];
                if (escape_next)
                {
                    escape_next = false;
                }
                else if (string_char == '\\')
                {
                    escape_next = true;
                }
                else if (string_char == '\"')
                {
                    parse_state.char_index += 1; // Skips close quotation mark.
                    break;
                }
                parse_state.char_index += 1;
                token->size += 1;
            }
        }
        else if (c == 'n')
        {
            // Every literal token will have its own node.
            if (is_array)
            {
                json->nodes.size += 1;
            }

            JSONToken* token = Push(&tokens);
            token->type  = JSONTokenType::NULL_;
            token->index = parse_state.char_index;
            token->size  = 4;

            parse_state.char_index += token->size;

            // Parsing ends on char after value; no need to increment parse_state.char_index.
        }
        else if (c == 't')
        {
            // Every literal token will have its own node.
            if (is_array)
            {
                json->nodes.size += 1;
            }

            JSONToken* token = Push(&tokens);
            token->type  = JSONTokenType::BOOLEAN;
            token->index = parse_state.char_index;
            token->size  = 4;

            parse_state.char_index += token->size;

            // Parsing ends on char after value; no need to increment parse_state.char_index.
        }
        else if (c == 'f')
        {
            // Every literal token will have its own node.
            if (is_array)
            {
                json->nodes.size += 1;
            }

            JSONToken* token = Push(&tokens);
            token->type  = JSONTokenType::BOOLEAN;
            token->index = parse_state.char_index;
            token->size  = 5;

            parse_state.char_index += token->size;

            // Parsing ends on char after value; no need to increment parse_state.char_index.
        }
        else if (ASCII_NUMERIC[c] || c == '-')
        {
            // Every numeric token will have its own node.
            if (is_array)
            {
                json->nodes.size += 1;
            }

            JSONToken* token = Push(&tokens);
            token->type  = JSONTokenType::UINT32;
            token->index = parse_state.char_index;
            token->size  = 1; // First char is start of numeric value.

            parse_state.char_index += 1;

            // Parse char after negative sign.
            if (c == '-')
            {
                token->type = JSONTokenType::SINT32;
                parse_state.char_index += 1;
                token->size += 1;
            }

            // Parse rest of numeric value.
            while (parse_state.char_index < json_file->count)
            {
                char num_char = json_file->data[parse_state.char_index];
                if (num_char == '.')
                {
                    token->type = JSONTokenType::FLOAT32;
                }
                else if (num_char == 'e')
                {
                    parse_state.char_index += 1;
                    parse_state.column += 1;
                    num_char = json_file->data[parse_state.char_index];
                    if (num_char == '-' || num_char == '+')
                    {
                        parse_state.char_index += 1;
                        parse_state.column += 1;
                    }
                    while (parse_state.char_index < json_file->count)
                    {
                        num_char = json_file->data[parse_state.char_index];
                        if (!ASCII_NUMERIC[num_char])
                        {
                            break;
                        }
                        parse_state.char_index += 1;
                        parse_state.column += 1;
                    }
                    break;
                }
                else if (!ASCII_NUMERIC[num_char])
                {
                    break;
                }
                parse_state.char_index += 1;
                token->size += 1;
            }

            // Parsing ends on char after value; no need to increment parse_state.char_index.
        }
        else if (JSON_SKIPPABLE[c])
        {
            // Skip
            parse_state.char_index += 1;
        }
        else if (JSON_SYMBOL[c])
        {
            JSONToken* token = Push(&tokens);
            token->type  = JSON_TOKEN_TYPE_SYMBOL[c];
            token->index = parse_state.char_index;
            token->size  = 1; // Symbols are always 1 character.

            // Track if token is in array or object for calculating node count.
            if (token->type == JSONTokenType::OPEN_SQUARE_BRACKET)
            {
                if (is_array)
                {
                    json->nodes.size += 1;
                }

                Push(&is_array_stack, true);
                is_array = true;
            }
            else if (token->type == JSONTokenType::OPEN_CURLY_BRACKET)
            {
                if (is_array)
                {
                    json->nodes.size += 1;
                }

                Push(&is_array_stack, false);
                is_array = false;
            }
            else if (token->type == JSONTokenType::CLOSE_SQUARE_BRACKET ||
                     token->type == JSONTokenType::CLOSE_CURLY_BRACKET)
            {
                is_array_stack.count -= 1;
                is_array = is_array_stack.data[is_array_stack.count - 1];
            }
            else if (!is_array && token->type == JSONTokenType::COLON)
            {
                // All nodes within an object will have a colon to separate key & value.
                json->nodes.size += 1;
                json->max_keys += 1;
            }

            // Parsing ends on last char of token; skip to next.
            parse_state.char_index += 1;
        }
    }
    return tokens;
}

void FindCloseBracket(JSONTokenType open_bracket_type, Array<JSONToken>* tokens, uint32* token_index)
{
    JSONTokenType close_bracket_type = (JSONTokenType)((uint32)open_bracket_type + 1);
    uint32 nesting_level = 0;
    while (*token_index <= tokens->count)
    {
        JSONToken* token = &tokens->data[*token_index];
        if (token->type == open_bracket_type)
        {
            nesting_level += 1;
        }
        else if (token->type == close_bracket_type)
        {
            if (nesting_level == 0)
            {
                *token_index += 1;
                return;
            }
            nesting_level -= 1;
        }
        *token_index += 1;
    }

    CTK_FATAL("reached end of tokens without finding close bracket type %s for open bracket type %s",
              TokenTypeName(close_bracket_type), TokenTypeName(open_bracket_type));
}

void ParseValue(JSON*               json,
                String*             json_file,
                Array<JSONToken>*   tokens,
                JSONNodeParseState* parse_state,
                uint32*             token_index,
                JSONNode*           node,
                uint32              node_index)
{
    JSONToken* value_token = &tokens->data[*token_index];
    *token_index += 1;
    if (value_token->type == JSONTokenType::OPEN_SQUARE_BRACKET)
    {
        // Store nodex index and token index for parsing container later.
        JSONListState* list_state = Push(&parse_state->list_states);
        list_state->token_index = *token_index;
        list_state->node_index  = node_index;

        node->type = JSONNodeType::ARRAY;
        FindCloseBracket(JSONTokenType::OPEN_SQUARE_BRACKET, tokens, token_index);
    }
    else if (value_token->type == JSONTokenType::OPEN_CURLY_BRACKET)
    {
        // Store nodex index and token index for parsing container later.
        JSONListState* list_state = Push(&parse_state->list_states);
        list_state->token_index = *token_index;
        list_state->node_index  = node_index;

        node->type = JSONNodeType::OBJECT;
        FindCloseBracket(JSONTokenType::OPEN_CURLY_BRACKET, tokens, token_index);
    }
    else if (value_token->type == JSONTokenType::UINT32)
    {
        node->type       = JSONNodeType::UINT32;
        node->num_uint32 = ToUInt32(&json_file->data[value_token->index], value_token->size);
    }
    else if (value_token->type == JSONTokenType::SINT32)
    {
        node->type       = JSONNodeType::SINT32;
        node->num_sint32 = ToSInt32(&json_file->data[value_token->index], value_token->size);

    }
    else if (value_token->type == JSONTokenType::FLOAT32)
    {
        node->type        = JSONNodeType::FLOAT32;
        node->num_float32 = ToFloat32(&json_file->data[value_token->index], value_token->size);

    }
    else if (value_token->type == JSONTokenType::STRING)
    {
        parse_state->value_offset -= value_token->size;

        node->type         = JSONNodeType::STRING;
        node->string.data  = &json->string_buffer[parse_state->value_offset];

        // Copy token string data to string buffer, evaluating escape sequences to their char values.
        String* node_string = &node->string;
        uint32 token_string_start = value_token->index;
        uint32 token_string_index = value_token->index;
        uint32 token_string_size  = 0;
        uint32 value_token_end    = value_token->index + value_token->size;
        char token_char           = 0;
        while (true)
        {
            if (token_string_index >= value_token_end)
            {
                memcpy(&node_string->data[node_string->size], &json_file->data[token_string_start], token_string_size);
                node_string->size += token_string_size;
                break; // Parsing reached end of string.
            }

            token_char = json_file->data[token_string_index];
            if (token_char == '\\')
            {
                memcpy(&node_string->data[node_string->size], &json_file->data[token_string_start], token_string_size);
                node_string->size += token_string_size;
                token_string_index += 1;
                token_char = json_file->data[token_string_index];
                if (token_char == 'n')
                {
                    node_string->data[node_string->size] = '\n';
                }
                else if (token_char == 'r')
                {
                    node_string->data[node_string->size] = '\r';
                }
                else if (token_char == 't')
                {
                    node_string->data[node_string->size] = '\t';
                }
                else if (token_char == '\\')
                {
                    node_string->data[node_string->size] = '\\';
                }
                else if (token_char == '\"')
                {
                    node_string->data[node_string->size] = '\"';
                }
                else if (token_char == '\0')
                {
                    node_string->data[node_string->size] = '\0';
                }
                else
                {
                    CTK_FATAL("invalid escape char: '%c' (%u)", token_char, token_char);
                }

                // Beging new region.
                node_string->size += 1;
                token_string_index += 1;
                token_string_start = token_string_index;
                token_string_size  = 0;
            }
            else
            {
                token_string_index += 1;
                token_string_size += 1;
            }
        }
        node_string->count = node_string->size;
    }
    else if (value_token->type == JSONTokenType::BOOLEAN)
    {
        node->type    = JSONNodeType::BOOLEAN;
        node->boolean = ToBool(&json_file->data[value_token->index], value_token->size);
    }
    else if (value_token->type == JSONTokenType::NULL_)
    {
        node->type = JSONNodeType::NULL_;
        // Leave node value zero-initialized.
    }
    else
    {
        CTK_FATAL("invalid value token: '%.*s'", value_token->size, &json_file->data[value_token->index]);
    }
}

uint32 ParseArrayChildren(JSON*               json,
                          String*             json_file,
                          Array<JSONToken>*   tokens,
                          JSONNodeParseState* parse_state,
                          uint32              token_index)
{
    uint32 child_count = 0;
    while (true)
    {
        if (token_index >= tokens->count)
        {
            CTK_FATAL("reached end of tokens without finding close bracket for array");
        }

        if (tokens->data[token_index].type == JSONTokenType::COMMA)
        {
            // Skip comma after value.
            token_index += 1;
        }

        if (tokens->data[token_index].type == JSONTokenType::CLOSE_SQUARE_BRACKET)
        {
            // Found close bracket for array, end parsing.
            break;
        }

        // Initialize key and write key string to string buffer at key_offset.
        child_count += 1;
        uint32 child_node_index = json->nodes.count;
        JSONNode* child_node = Push(&json->nodes);

        // Parse value for node.
        ParseValue(json, json_file, tokens, parse_state, &token_index, child_node, child_node_index);
    }
    return child_count;
}

uint32 ParseObjectChildren(JSON*               json,
                           String*             json_file,
                           Array<JSONToken>*   tokens,
                           JSONNodeParseState* parse_state,
                           uint32              token_index)
{
    uint32 original_token_index = token_index;
    uint32 child_count = 0;
    while (true)
    {
        if (token_index >= tokens->count)
        {
            CTK_FATAL("reached end of tokens without finding close bracket for object");
        }

        if (tokens->data[token_index].type == JSONTokenType::COMMA)
        {
            // Skip comma after value.
            token_index += 1;
        }

        if (tokens->data[token_index].type == JSONTokenType::CLOSE_CURLY_BRACKET)
        {
            // Found close bracket for object, end parsing.
            break;
        }

        // Parse key token.
        if (token_index >= tokens->count)
        {
            CTK_FATAL("expected key token: no tokens remain");
        }
        JSONToken* key_token = &tokens->data[token_index];
        token_index += 1;
        if (key_token->type != JSONTokenType::STRING)
        {
            CTK_FATAL("expected token for key to be STRING type: was %s", TokenTypeName(key_token->type));
        }
        if (parse_state->key_index >= json->max_keys)
        {
            CTK_FATAL("can't parse object child: already at max of %u keys", json->max_keys);
        }

        // Parse colon token.
        if (token_index >= tokens->count)
        {
            CTK_FATAL("expected colon token: no tokens remain");
        }
        JSONToken* colon_token = &tokens->data[token_index];
        token_index += 1;
        if (colon_token->type != JSONTokenType::COLON)
        {
            CTK_FATAL("expected token for colon to be COLON type: was %s", TokenTypeName(colon_token->type));
        }

        // Initialize key and write key string to string buffer at key_offset.
        child_count += 1;
        uint32 child_node_index = json->nodes.count;
        JSONNode* child_node = Push(&json->nodes);
        child_node->key.data  = &json->string_buffer[parse_state->key_offset];
        child_node->key.size  = key_token->size;
        child_node->key.count = key_token->size;
        child_node->key_index = parse_state->key_index;
        memcpy(child_node->key.data, &json_file->data[key_token->index], key_token->size);

        // Store key size and index of key's node.
        json->key_sizes  [parse_state->key_index] = key_token->size;
        json->key_offsets[parse_state->key_index] = parse_state->key_offset;

        // Update key parse state.
        parse_state->key_index  += 1;
        parse_state->key_offset += key_token->size;

        // Parse value for node.
        ParseValue(json, json_file, tokens, parse_state, &token_index, child_node, child_node_index);
    }
    return child_count;
}

void ParseNodes(JSON* json, String* json_file, Array<JSONToken>* tokens)
{
    json->nodes         = CreateArray<JSONNode>(json->allocator, json->nodes.size);
    json->string_buffer = Allocate<char>  (json->allocator, json->string_buffer_size);
    json->key_sizes     = Allocate<uint32>(json->allocator, json->max_keys);
    json->key_offsets   = Allocate<uint32>(json->allocator, json->max_keys);

    // Parse nodes from tokens.
    JSONNodeParseState parse_state =
    {
        .list_states  = CreateArray<JSONListState>(json->allocator, json->nodes.size),
        .key_index    = 0,
        .key_offset   = 0,
        .value_offset = json->string_buffer_size,
    };

    // Parse root nodes.
    JSONToken* first_token = GetPtr(tokens, 0);
    JSONNode* root_node = &json->root_node;
    if (first_token->type == JSONTokenType::OPEN_SQUARE_BRACKET)
    {
        root_node->type = JSONNodeType::ARRAY;
        root_node->list.size = ParseArrayChildren(json, json_file, tokens, &parse_state, 1);
    }
    else if (first_token->type == JSONTokenType::OPEN_CURLY_BRACKET)
    {
        root_node->type = JSONNodeType::OBJECT;
        root_node->list.size = ParseObjectChildren(json, json_file, tokens, &parse_state, 1);
    }
    else
    {
        CTK_FATAL("JSON file must start with { or [");
    }

    // Parse all child lists of root.
    for (uint32 i = 0; i < parse_state.list_states.count; i += 1)
    {
        // Don't use pointers to container state or node as both arrays can be reallocated during ParseXChildren().
        uint32 node_index  = parse_state.list_states.data[i].node_index;
        uint32 token_index = parse_state.list_states.data[i].token_index;
        JSONNode* node = &json->nodes.data[node_index];
        node->list.node_index = json->nodes.count;
        if (node->type == JSONNodeType::OBJECT)
        {
            node->list.key_index = parse_state.key_index;
            node->list.size = ParseObjectChildren(json, json_file, tokens, &parse_state, token_index);
        }
        else if (node->type == JSONNodeType::ARRAY)
        {
            node->list.size = ParseArrayChildren(json, json_file, tokens, &parse_state, token_index);
        }
        else
        {
            CTK_FATAL("expected container node to be ARRAY or OBJECT type: was %s",
                      NodeTypeName(node->type));
        }
    }

    DestroyArray(&parse_state.list_states);
}

/// Debug
////////////////////////////////////////////////////////////
void PrintValue(JSON* json, JSONNode* node, PrintChildFuncs* print_child_funcs, uint32 tabs);

void PrintObjectChildDebug(JSONNode* node, uint32 node_index, uint32 key_index, uint32 child_index)
{
    Print("(type:%7s, node_index:%4u, key_index:%4u) \"%.*s\": ",
          NodeTypeName(node->type),
          node_index,
          key_index,
          node->key.size,
          node->key.data);
    if (node->type == JSONNodeType::OBJECT)
    {
        Print("list:(node_index:%4u, key_index:%4u, size:%4u) ",
              node->list.node_index,
              node->list.key_index,
              node->list.size);
    }
}

void PrintObjectChild(JSONNode* node, uint32 node_index, uint32 key_index, uint32 child_index)
{
    Print("\"%.*s\": ", node->key.size, node->key.data);
}

void PrintArrayChildDebug(JSONNode* node, uint32 node_index, uint32 key_index, uint32 child_index)
{
    Print("(type:%7s, node_index:%4u) [%3u]: ",
          NodeTypeName(node->type),
          node_index,
          child_index);
    if (node->type == JSONNodeType::OBJECT)
    {
        Print("list:(node_index:%4u, key_index:%4u, size:%4u) ",
              node->list.node_index,
              node->list.key_index,
              node->list.size);
    }
}

void PrintArrayChildIndex(JSONNode* node, uint32 node_index, uint32 key_index, uint32 child_index)
{
    Print("[%3u]: ", child_index);
}

void PrintArrayChild(JSONNode* node, uint32 node_index, uint32 key_index, uint32 child_index)
{
}

void PrintList(JSON*            json,
               JSONNodeList*    list,
               PrintChildFunc   PrintChild,
               PrintChildFuncs* print_child_funcs,
               uint32           tabs)
{
    if (list->size > 0)
    {
        PrintLine();
    }
    for (uint32 child_index = 0; child_index < list->size; child_index += 1)
    {
        uint32 node_index = list->node_index + child_index;
        uint32 key_index  = list->key_index  + child_index;
        JSONNode* node = &json->nodes.data[node_index];
        PrintTabs(tabs);
        PrintChild(node, node_index, key_index, child_index);
        PrintValue(json, node, print_child_funcs, tabs);
    }
    if (list->size > 0)
    {
        PrintTabs(tabs - 1);
    }
}

void PrintValue(JSON* json, JSONNode* node, PrintChildFuncs* print_child_funcs, uint32 tabs)
{
    if (node->type == JSONNodeType::UINT32)
    {
        Print("%u", node->num_uint32);
    }
    else if (node->type == JSONNodeType::SINT32)
    {
        Print("%d", node->num_sint32);
    }
    else if (node->type == JSONNodeType::FLOAT32)
    {
        Print("%f", node->num_float32);
    }
    else if (node->type == JSONNodeType::STRING)
    {
        Print("\"%.*s\"", node->string.size, node->string.data);
    }
    else if (node->type == JSONNodeType::BOOLEAN)
    {
        Print("%s", node->boolean ? "true" : "false");
    }
    else if (node->type == JSONNodeType::NULL_)
    {
        Print("null");
    }
    else if (node->type == JSONNodeType::ARRAY)
    {
        Print('[');
        PrintList(json, &node->list, print_child_funcs->array, print_child_funcs, tabs + 1);
        Print(']');
    }
    else if (node->type == JSONNodeType::OBJECT)
    {
        Print('{');
        PrintList(json, &node->list, print_child_funcs->object, print_child_funcs, tabs + 1);
        Print('}');
    }
    PrintLine(",");
}

void PrintJSON(JSON* json, PrintChildFuncs* print_child_funcs)
{
    PrintValue(json, &json->root_node, print_child_funcs, 0);
}

void PrintNode(JSON* json, JSONNode* node)
{
    PrintLine("key:   %.*s", node->key.size, node->key.data);
    PrintLine("type:  %s", NodeTypeName(node->type));
    Print("value: ");
    if (node->type == JSONNodeType::UINT32)
    {
        Print("%u", node->num_uint32);
    }
    else if (node->type == JSONNodeType::SINT32)
    {
        Print("%d", node->num_sint32);
    }
    else if (node->type == JSONNodeType::FLOAT32)
    {
        Print("%f", node->num_float32);
    }
    else if (node->type == JSONNodeType::STRING)
    {
        Print("\"%.*s\"", node->string.size, node->string.data);
    }
    else if (node->type == JSONNodeType::BOOLEAN)
    {
        Print("%s", node->boolean ? "true" : "false");
    }
    else if (node->type == JSONNodeType::NULL_)
    {
        Print("null");
    }
    else if (node->type == JSONNodeType::ARRAY)
    {
        Print("[]");
    }
    else if (node->type == JSONNodeType::OBJECT)
    {
        Print("{}");
    }
    Print("\n\n");
}

void PrintKeys(JSON* json, JSONNode* node)
{
    CTK_ASSERT(node->type == JSONNodeType::OBJECT);
    JSONNodeList* list = &node->list;
    for (uint32 child_index = 0; child_index < list->size; child_index += 1)
    {
        uint32 key_index = list->key_index + child_index;
        uint32 key_size = json->key_sizes[key_index];
        uint32 key_offset = json->key_offsets[key_index];
        PrintLine("[%4u] %.*s (node_index:%4u)",
                  key_index,
                  key_size,
                  &json->string_buffer[key_offset],
                  list->node_index + child_index);
    }
}

/// Interface
////////////////////////////////////////////////////////////
JSON LoadJSON(Allocator* allocator, const char* path)
{
    CTK_ASSERT(allocator != NULL);
    CTK_ASSERT(allocator->Deallocate != NULL);

    JSON json = {};
    json.allocator = allocator;

    // Load File
    String json_file = ReadFile<char>(json.allocator, path);
    if (json_file.count == 0)
    {
        return json;
    }

    Array<JSONToken> tokens = ParseTokens(&json, &json_file);
    if (tokens.count == 0)
    {
        DestroyString(&json_file);
        return json;
    }
// for (uint32 i = 0; i < tokens.count; i += 1)
// {
//     JSONToken* token = &tokens.data[i];
//     PrintLine("[%4u] %20s: (%.*s)", i, TokenTypeName(token->type), token->size, &json_file.data[token->index]);
// }

    ParseNodes(&json, &json_file, &tokens);
// PrintChildFuncs print_child_funcs =
// {
//     .array  = PrintArrayChildDebug,
//     .object = PrintObjectChildDebug,
// };
// PrintJSON(&json, &print_child_funcs);
// PrintASCIIString(json.string_buffer, json.string_buffer_size, 128);
// Print("\n\n");

    // Cleanup
    DestroyString(&json_file);
    DestroyArray(&tokens);

    return json;
}

void DestroyJSON(JSON* json)
{
    DestroyArray(&json->nodes);
    if (json->string_buffer != NULL) { Deallocate(json->allocator, json->string_buffer); }
    if (json->key_sizes     != NULL) { Deallocate(json->allocator, json->key_sizes);     }
    if (json->key_offsets   != NULL) { Deallocate(json->allocator, json->key_offsets);   }
    *json = {};
}

JSONNode* FindNode(JSON* json, JSONNode* node, const char* key, uint32 key_size)
{
    CTK_ASSERT(node->type == JSONNodeType::OBJECT);

    JSONNodeList* list = &node->list;
    for (uint32 child_index = 0; child_index < list->size; child_index += 1)
    {
        uint32 key_index = list->key_index + child_index;
        if (json->key_sizes[key_index] == key_size)
        {
            uint32 key_offset = json->key_offsets[key_index];
            if (StringsMatch(&json->string_buffer[key_offset], key, key_size))
            {
                return &json->nodes.data[list->node_index + child_index];
            }
        }
    }

    return NULL;
}

JSONNode* FindNode(JSON* json, JSONNode* node, const char* key)
{
    return FindNode(json, node, key, StringSize(key));
}

JSONNode* GetNode(JSON* json, JSONNode* node, const char* key, uint32 key_size)
{
    JSONNode* child_node = FindNode(json, node, key, key_size);
    if (child_node == NULL)
    {
        CTK_FATAL("can't get child node with key \"%.*s\" in object \"%.*s\"",
                  key_size,
                  key,
                  node->key.size,
                  node->key.data);
    }
    return child_node;
}

JSONNode* GetNode(JSON* json, JSONNode* node, const char* key)
{
    return GetNode(json, node, key, StringSize(key));
}

JSONNode* GetNode(JSON* json, JSONNode* node, uint32 index)
{
    CTK_ASSERT(node->type == JSONNodeType::ARRAY || node->type == JSONNodeType::OBJECT);

    JSONNodeList* list = &node->list;
    if (index >= list->size)
    {
        CTK_FATAL("can't get child node in \"%.*s\": index %u exceeds child count of %u",
                  node->key.size,
                  node->key.data,
                  index,
                  list->size);
    }
    return &json->nodes.data[list->node_index + index];
}

JSONNode* GetObject(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::OBJECT);
    return node;
}

JSONNode* GetObject(JSON* json, const char* key)
{
    return GetObject(json, &json->root_node, key);
}

JSONNode* GetArray(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::ARRAY);
    return node;
}

JSONNode* GetArray(JSON* json, const char* key)
{
    return GetArray(json, &json->root_node, key);
}

uint32 GetUInt32(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::UINT32);
    return node->num_uint32;
}

uint32 GetUInt32(JSON* json, const char* key)
{
    return GetUInt32(json, &json->root_node, key);
}

sint32 GetSInt32(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::SINT32);
    return node->num_sint32;
}

sint32 GetSInt32(JSON* json, const char* key)
{
    return GetSInt32(json, &json->root_node, key);
}

float32 GetFloat32(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::FLOAT32);
    return node->num_float32;
}

float32 GetFloat32(JSON* json, const char* key)
{
    return GetFloat32(json, &json->root_node, key);
}

String* GetString(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::STRING);
    return &node->string;
}

String* GetString(JSON* json, const char* key)
{
    return GetString(json, &json->root_node, key);
}

bool GetBoolean(JSON* json, JSONNode* node, const char* key)
{
    node = GetNode(json, node, key);
    CTK_ASSERT(node->type == JSONNodeType::BOOLEAN);
    return node->boolean;
}

bool GetBoolean(JSON* json, const char* key)
{
    return GetBoolean(json, &json->root_node, key);
}

JSONNode* GetObject(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::OBJECT);
    return node;
}

JSONNode* GetObject(JSON* json, uint32 index)
{
    return GetObject(json, &json->root_node, index);
}

JSONNode* GetArray(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::ARRAY);
    return node;
}

JSONNode* GetArray(JSON* json, uint32 index)
{
    return GetArray(json, &json->root_node, index);
}

uint32 GetUInt32(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::UINT32);
    return node->num_uint32;
}

uint32 GetUInt32(JSON* json, uint32 index)
{
    return GetUInt32(json, &json->root_node, index);
}

sint32 GetSInt32(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::SINT32);
    return node->num_sint32;
}

sint32 GetSInt32(JSON* json, uint32 index)
{
    return GetSInt32(json, &json->root_node, index);
}

float32 GetFloat32(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::FLOAT32);
    return node->num_float32;
}

float32 GetFloat32(JSON* json, uint32 index)
{
    return GetFloat32(json, &json->root_node, index);
}

String* GetString(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::STRING);
    return &node->string;
}

String* GetString(JSON* json, uint32 index)
{
    return GetString(json, &json->root_node, index);
}

bool GetBoolean(JSON* json, JSONNode* node, uint32 index)
{
    node = GetNode(json, node, index);
    CTK_ASSERT(node->type == JSONNodeType::BOOLEAN);
    return node->boolean;
}

bool GetBoolean(JSON* json, uint32 index)
{
    return GetBoolean(json, &json->root_node, index);
}

JSONNode* SearchNode(JSON* json, JSONNode* node, const char* search)
{
    uint32 search_size     = StringSize(search);
    uint32 term_offset     = 0;
    uint32 term_size       = 0;
    uint32 term_char_index = 0;
    char   term_char       = 0;
    while (true)
    {
        term_char_index = term_offset + term_size;
        if (term_char_index >= search_size)
        {
            break;
        }
        term_char = search[term_char_index];

        // Both key parsing and subscript parsing can end on .
        if (term_char == '.')
        {
            term_offset += 1;
        }
        else if (term_char == '[')
        {
            term_offset += 1;
            while (true)
            {
                term_char_index = term_offset + term_size;
                if (term_char_index >= search_size)
                {
                    CTK_FATAL("reached end of search term parsing subscript index");
                }
                term_char = search[term_char_index];

                if (term_char == ']')
                {
                    break;
                }
                if (!ASCII_NUMERIC[term_char])
                {
                    CTK_FATAL("non-numeric character in subscript index at search term column %u", term_offset)
                }
                term_size += 1;
            }
            if (term_size == 0)
            {
                CTK_FATAL("empty subscript at search term column %u", term_offset - 1)
            }
            node = GetNode(json, node, ToUInt32(&search[term_offset]));
            term_offset += term_size + 1; // +1 because subscript parsing always ends on close square bracket.
            term_size = 0;
        }
        else
        {
            while (true)
            {
                if (term_char == '.' || term_char == '[')
                {
                    break;
                }
                term_size += 1;

                term_char_index = term_offset + term_size;
                if (term_char_index >= search_size)
                {
                    break;
                }
                term_char = search[term_char_index];
            }
            if (term_size == 0)
            {
                CTK_FATAL("empty key in search term column %u", term_offset)
            }
            node = GetNode(json, node, &search[term_offset], term_size);
            term_offset += term_size;
            term_size = 0;
        }
    }
    return node;
}

JSONNode* SearchNode(JSON* json, const char* search)
{
    return SearchNode(json, &json->root_node, search);
}

JSONNode* SearchObject(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::OBJECT);
    return node;
}

JSONNode* SearchObject(JSON* json, const char* search)
{
    return SearchObject(json, &json->root_node, search);
}

JSONNode* SearchArray(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::ARRAY);
    return node;
}

JSONNode* SearchArray(JSON* json, const char* search)
{
    return SearchArray(json, &json->root_node, search);
}

uint32 SearchUInt32(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::UINT32);
    return node->num_uint32;
}

uint32 SearchUInt32(JSON* json, const char* search)
{
    return SearchUInt32(json, &json->root_node, search);
}

sint32 SearchSInt32(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::SINT32);
    return node->num_sint32;
}

sint32 SearchSInt32(JSON* json, const char* search)
{
    return SearchSInt32(json, &json->root_node, search);
}

float32 SearchFloat32(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::FLOAT32);
    return node->num_float32;
}

float32 SearchFloat32(JSON* json, const char* search)
{
    return SearchFloat32(json, &json->root_node, search);
}

String* SearchString(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::STRING);
    return &node->string;
}

String* SearchString(JSON* json, const char* search)
{
    return SearchString(json, &json->root_node, search);
}

bool SearchBoolean(JSON* json, JSONNode* node, const char* search)
{
    node = SearchNode(json, node, search);
    CTK_ASSERT(node->type == JSONNodeType::BOOLEAN);
    return node->boolean;
}

bool SearchBoolean(JSON* json, const char* search)
{
    return SearchBoolean(json, &json->root_node, search);
}
