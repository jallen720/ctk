static cstr
TokenTypeDebugTag(token::type Type)
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
PrintTokens(array<token> *Tokens)
{
    for(u32 TokenIndex = 0; TokenIndex < Tokens->Count; TokenIndex++)
    {
        token *Token = At(Tokens, TokenIndex);
        PrintLine("[%u] %s <%.*s>", TokenIndex, TokenTypeDebugTag(Token->Type), Token->Size, Token->Data);
    }
}

static void
PrintData(data *Data, u32 TabCount = 0)
{
    PrintTabs(TabCount);
    if(Data->Key.Count > 0)
    {
        Print("%.*s: ", Data->Key.Count, Data->Key.Data);
        switch(Data->Type)
        {
            case data::type::ARRAY:
            case data::type::OBJECT:
            {
                if(Data->Children.Count > 0)
                {
                    PrintLine();
                    PrintTabs(TabCount);
                }
            }
        }
    }
    switch(Data->Type)
    {
        case data::type::STRING:
        {
            PrintLine("\"%.*s\"", Data->Value.Count, Data->Value.Data);
            break;
        }
        case data::type::ARRAY:
        {
            if(Data->Children.Count == 0)
            {
                PrintLine("[]");
            }
            else
            {
                PrintLine("[");
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    PrintData(Data->Children + ChildIndex, TabCount + 1);
                }
                PrintTabs(TabCount);
                PrintLine("]");
            }
            break;
        }
        case data::type::OBJECT:
        {
            if(Data->Children.Count == 0)
            {
                PrintLine("{}");
            }
            else
            {
                PrintLine("{");
                for(u32 ChildIndex = 0; ChildIndex < Data->Children.Count; ChildIndex++)
                {
                    PrintData(Data->Children + ChildIndex, TabCount + 1);
                }
                PrintTabs(TabCount);
                PrintLine("}");
            }
            break;
        }
    }
}
