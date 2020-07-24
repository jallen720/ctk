static cstr
token_type_debug_tag(token::type Type)
{
    switch(Type)
    {
        case token::type::TEXT:         return "TEXT        ";
        case token::type::STRING:       return "STRING      ";
        case token::type::ARRAY_OPEN:   return "ARRAY_OPEN  ";
        case token::type::ARRAY_CLOSE:  return "ARRAY_CLOSE ";
        case token::type::OBJECT_OPEN:  return "OBJECT_OPEN ";
        case token::type::OBJECT_CLOSE: return "OBJECT_CLOSE";
        default: CTK_FATAL("unknown token type")
    };
}

static void
print_tokens(array<token> *Tokens)
{
    for(u32 TokenIndex = 0; TokenIndex < Tokens->Count; TokenIndex++)
    {
        token *Token = at(Tokens, TokenIndex);
        print_line("[%u] %s <%.*s>", TokenIndex, token_type_debug_tag(Token->Type), Token->Size, Token->Data);
    }
}

static void
print_data(data *Data, u32 TabCount = 0)
{
    print_tabs(TabCount);
    if(Data->Key.Count > 0)
    {
        print("%.*s: ", Data->Key.Count, Data->Key.Data);
        switch(Data->Type)
        {
            case data::type::ARRAY:
            case data::type::OBJECT:
            {
                if(Data->Children.Count > 0)
                {
                    print_line();
                    print_tabs(TabCount);
                }
            }
        }
    }
    switch(Data->Type)
    {
        case data::type::STRING:
        {
            print_line("\"%.*s\"", Data->Value.Count, Data->Value.Data);
            break;
        }
        case data::type::ARRAY:
        {
            if(Data->Children.Count == 0)
            {
                print_line("[]");
            }
            else
            {
                print_line("[");
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    print_data(Data->Children + ChildIndex, TabCount + 1);
                }
                print_tabs(TabCount);
                print_line("]");
            }
            break;
        }
        case data::type::OBJECT:
        {
            if(Data->Children.Count == 0)
            {
                print_line("{}");
            }
            else
            {
                print_line("{");
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    print_data(Data->Children + ChildIndex, TabCount + 1);
                }
                print_tabs(TabCount);
                print_line("}");
            }
            break;
        }
    }
}
