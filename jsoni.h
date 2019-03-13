#ifndef JSONI_H
#define JSONI_H

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <cassert>

// util types
struct Location;
struct Visitor;
struct PrettyPrint;
struct Error;
struct ParseResult;

// json types
struct Value;
struct Object;
struct Array;
struct String;
struct Number;
struct Bool;
struct Null;
struct Int;

struct Location
{
  Location();
  explicit Location(int l);
  Location(int l, int c);

  int line;
  int column;
};

std::ostream& operator<<(std::ostream& s, const Location& location);

struct Visitor
{
  virtual void VisitObject (Object* object ) = 0;
  virtual void VisitArray  (Array * array  ) = 0;
  virtual void VisitString (String* string ) = 0;
  virtual void VisitNumber (Number* number ) = 0;
  virtual void VisitBool   (Bool  * boolean) = 0;
  virtual void VisitNull   (Null  * null   ) = 0;
  virtual void VisitInt    (Int   * integer);
};

struct PrettyPrint
{
  // todo: add options here

  static PrettyPrint Compact();
};

struct Value
{
  Location location;

  virtual ~Value();

  virtual void Visit(Visitor* visitor) = 0;

  virtual Object* AsObject();
  virtual Array * AsArray ();
  virtual String* AsString();
  virtual Number* AsNumber();
  virtual Bool  * AsBool  ();
  virtual Null  * AsNull  ();
  virtual Int   * AsInt();
};

std::string ToString(Value* value, const PrettyPrint& pp);

struct Object : public Value
{
  std::map<std::string, std::shared_ptr<Value>> object;

  void Visit(Visitor* visitor) override;

  Object* AsObject() override;
};

struct Array : public Value
{
  std::vector<std::shared_ptr<Value>> array;

  void Visit(Visitor* visitor) override;

  Array* AsArray() override;
};

struct String : public Value
{
  std::string string;

  void Visit(Visitor* visitor) override;

  String* AsString() override;

  explicit String(const std::string& s="");
};

struct Number : public Value
{
  double number;

  void Visit(Visitor* visitor) override;

  Number* AsNumber() override;

  explicit Number(double d);
};

struct Bool : public Value
{
  bool boolean;

  void Visit(Visitor* visitor) override;

  Bool* AsBool() override;

  explicit Bool(bool b);
};

struct Null : public Value
{
  void Visit(Visitor* visitor) override;

  Null* AsNull() override;
};

struct Int : public Number
{
  int integer;

  void Visit(Visitor* visitor) override;

  Int* AsInt() override;

  explicit Int(int i);
};

struct Error
{
  enum class Type
  {
    Note,
    InvalidCharacter,
    UnclosedArray,
    UnclosedObject,
    NotEof,
    InvalidNumber,
    IllegalEscape,
    InvalidCharacterInString,
    UnexpectedEof,
    UnknownError
  };
  Type type;
  std::string message;
  Location location;

  Error(Type t, const std::string& m, const Location& l=Location());
};

std::ostream& operator<<(std::ostream& s, const Error::Type& type);

std::ostream& operator<<(std::ostream& s, const Error& error);

// todo: this structure seems weird, make it into something that feels less weird
struct ParseResult
{
  // contains errors if parsing failed
  std::vector<Error> errors;

  // is non-null is parsing succeeded
  std::shared_ptr<Value> value;

  bool HasError() const;

  operator bool() const;
};

std::ostream& operator<<(std::ostream& s, const ParseResult& result);

ParseResult Parse(const std::string& str);

#ifdef JSONI_IMPLEMENTATION

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation detail..

Location::Location() : line(-1), column(-1) {}
Location::Location(int l) : line(l), column(-1) {}
Location::Location(int l, int c) : line(l), column(c) {}

std::ostream& operator<<(std::ostream& s, const Location& location)
{
  s << "(" << location.line << ", " << location.column << ")";
  return s;
}

PrettyPrint PrettyPrint::Compact()
{
  return {};
}

Value::~Value() {}
Object* Value::AsObject() { return nullptr; }
Array * Value::AsArray () { return nullptr; }
String* Value::AsString() { return nullptr; }
Number* Value::AsNumber() { return nullptr; }
Bool  * Value::AsBool  () { return nullptr; }
Null  * Value::AsNull  () { return nullptr; }
Int   * Value::AsInt() { return nullptr; }

std::string ToString(Value* value, const PrettyPrint& pp)
{
  return "";
}

void Visitor::VisitInt(Int* integer) { VisitNumber(integer); }

void Object::Visit(Visitor* visitor) { visitor->VisitObject(this); }
Object* Object::AsObject() { return this; }

void Array::Visit(Visitor* visitor) { visitor->VisitArray(this); }
Array* Array::AsArray() { return this; }

void String::Visit(Visitor* visitor) { visitor->VisitString(this); }
String* String::AsString() { return this; }
String::String(const std::string& s) : string(s) {}

void Number::Visit(Visitor* visitor) { visitor->VisitNumber(this); }
Number* Number::AsNumber() { return this; }
Number::Number(double d) : number(d) {}

void Bool::Visit(Visitor* visitor) { visitor->VisitBool(this); }
Bool* Bool::AsBool() { return this; }
Bool::Bool(bool b) : boolean(b) {}

void Null::Visit(Visitor* visitor) { visitor->VisitNull(this); }
Null* Null::AsNull() { return this; }

void Int::Visit(Visitor* visitor) { visitor->VisitInt(this); }
Int* Int::AsInt() { return this; }
Int::Int(int i) : Number(i), integer(i) { }


Error::Error(Type t, const std::string& m, const Location& l) : type(t), message(m), location(l) {}

std::ostream& operator<<(std::ostream& s, const Error::Type& type)
{
  switch (type)
  {
#define CASE(x) case Error::Type::x: s << #x; break
    CASE(Note                     );
    CASE(InvalidCharacter         );
    CASE(UnclosedArray            );
    CASE(UnclosedObject           );
    CASE(NotEof                   );
    CASE(InvalidNumber            );
    CASE(IllegalEscape            );
    CASE(InvalidCharacterInString );
    CASE(UnexpectedEof            );
    CASE(UnknownError             );
#undef CASE
  default:
    s << "<unknown error>";
    break;
  }
  return s;
}

std::ostream& operator<<(std::ostream& s, const Error& error)
{
  s << error.type << error.location << ": " << error.message;
  return s;
}

bool ParseResult::HasError() const
{
  return value == nullptr;
}

ParseResult::operator bool() const
{
  return !HasError();
}

std::ostream& operator<<(std::ostream& s, const ParseResult& result)
{
  if (result.HasError())
  {
    s << "Errors:\n";
    for (const auto& e : result.errors)
    {
      s << "  " << e << "\n";
    }
  }
  else
  {
    s << "No errors detected, but currently unable to print data";
  }
  return s;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here comes the parser

// todo: change all these parsers to a better structure with loops instead of this while/switch/state structure

struct Parser;

struct Parser
{
  std::string str;
  int index = 0;

  explicit Parser(const std::string& s) : str(s) {}

  char Peek(int advance = 0) const
  {
    const int i = index + advance;
    if (index >= str.size()) return 0;
    return str[i];
  }

  bool HasMoreChar() const
  {
    return index < str.size();
  }

  int line = 0;
  int column = 0;

  char Read()
  {
    const char c = str[index];
    if (c == '\n')
    {
      line += 1;
      column = 0;
    }
    else
    {
      column += 1;
    }
    index += 1;
    return c;
  }
};

bool IsSpace(char c)
{
  if (c == ' ') return true;
  if (c == '\t') return true;
  if (c == '\n') return true;
  if (c == '\r') return true;
  return false;
}

void SkipSpaces(Parser* parser)
{
  while (IsSpace(parser->Peek()))
  {
    parser->Read();
  }
}

#define EXPECT(expected_string) do { char c = parser->Read(); if( c != expected_string[0] ) { AddError(result, parser, Error::Type::InvalidCharacter, "Expected character " expected_string); return nullptr; } SkipSpaces(parser); } while(false)

void AddError(ParseResult* result, Parser* parser, Error::Type type, const std::string& err)
{
  result->errors.push_back(Error{type, err, Location{parser->line, parser->column} });
  result->value = nullptr;
}

std::shared_ptr<Value > ParseValue  (ParseResult* result, Parser* parser);
std::shared_ptr<Object> ParseObject (ParseResult* result, Parser* parser);
std::shared_ptr<Array > ParseArray  (ParseResult* result, Parser* parser);
std::shared_ptr<String> ParseString (ParseResult* result, Parser* parser);
std::shared_ptr<Number> ParseNumber (ParseResult* result, Parser* parser);
// std::shared_ptr<Int   > ParseInt    (ParseResult* result, Parser* parser);

bool IsDigit09(char c)
{
  return c >= '0' && c <= '9';
}

bool IsHex(char c)
{
  return IsDigit09(c)
    || (c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    ;
}

bool IsDigit19(char c)
{
  return c >= '1' && c <= '9';
}

bool IsValidFirstDigit(char c);

std::shared_ptr<Value> ParseValue(ParseResult* result, Parser* parser)
{
  if (parser->Peek() == '[')
  {
    return ParseArray(result, parser);
  }

  if (parser->Peek() == '{')
  {
    return ParseObject(result, parser);
  }

  if (parser->Peek() == '"')
  {
    return ParseString(result, parser);
  }

  if (
    parser->Peek(0) == 't' &&
    parser->Peek(1) == 'r' &&
    parser->Peek(2) == 'u' &&
    parser->Peek(3) == 'e'
    )
  {
    parser->Read(); // t
    parser->Read(); // r
    parser->Read(); // u
    parser->Read(); // e
    SkipSpaces(parser);
    return std::make_shared<Bool>(true);
  }

  if (
    parser->Peek(0) == 'f' &&
    parser->Peek(1) == 'a' &&
    parser->Peek(2) == 'l' &&
    parser->Peek(3) == 's' &&
    parser->Peek(4) == 'e'
    )
  {
    parser->Read(); // f
    parser->Read(); // a
    parser->Read(); // l
    parser->Read(); // s
    parser->Read(); // e
    SkipSpaces(parser);
    return std::make_shared<Bool>(false);
  }

  if (
    parser->Peek(0) == 'n' &&
    parser->Peek(1) == 'u' &&
    parser->Peek(2) == 'l' &&
    parser->Peek(3) == 'l'
    )
  {
    parser->Read(); // n
    parser->Read(); // u
    parser->Read(); // l
    parser->Read(); // l
    SkipSpaces(parser);
    return std::make_shared<Null>();
  }

  if (IsValidFirstDigit(parser->Peek()))
  {
    return ParseNumber(result, parser);
  }

  AddError(result, parser, Error::Type::InvalidCharacter, "Unexpected character found");
  return nullptr;
}

std::shared_ptr<Array> ParseArray(ParseResult* result, Parser* parser)
{
  enum class State { ExpectStart, ExpectValue, ExpectComma, ExpectEnd, Ended };

  State state = State::ExpectStart;

  auto array = std::make_shared<Array>();

  while(state != State::Ended)
  { 
    switch (state)
    {
    case State::ExpectStart:
      EXPECT("[");
      if (parser->Peek() == ']')
      {
        state = State::ExpectEnd;
      }
      else
      {
        state = State::ExpectValue;
      }
      break;
    case State::ExpectValue:
      {
        auto v = ParseValue(result, parser);
        if (v == nullptr)
        {
          return nullptr;
        }
        else
        {
          array->array.push_back(v);
        }
        SkipSpaces(parser);
        if (parser->Peek() == ',')
        {
          state = State::ExpectComma;
        }
        else
        {
          state = State::ExpectEnd;
        }
      }
      break;
    case State::ExpectComma:
      EXPECT(",");
      state = State::ExpectValue;
      break;
    case State::ExpectEnd:
      EXPECT("]");
      state = State::Ended;
      break;
    case State::Ended:
      assert(false && "array loop should have ended");
      break;
    }
  }

  return array;
}

std::shared_ptr<Object> ParseObject(ParseResult* result, Parser* parser)
{
  enum class State { ExpectStart, ExpectValue, ExpectComma, ExpectEnd, Ended };

  State state = State::ExpectStart;

  auto object = std::make_shared<Object>();

  while (state != State::Ended)
  {
    switch (state)
    {
    case State::ExpectStart:
      EXPECT("{");
      if (parser->Peek() == '}')
      {
        state = State::ExpectEnd;
      }
      else
      {
        state = State::ExpectValue;
      }
      break;
    case State::ExpectValue:
    {
      auto s = ParseString(result, parser);
      if (s == nullptr)
      {
        return nullptr;
      }
      EXPECT(":");
      auto v = ParseValue(result, parser);
      if (v == nullptr)
      {
        return nullptr;
      }
      else
      {
        object->object[s->string] = v;
      }
      SkipSpaces(parser);
      if (parser->Peek() == ',')
      {
        state = State::ExpectComma;
      }
      else
      {
        state = State::ExpectEnd;
      }
    }
    break;
    case State::ExpectComma:
      EXPECT(",");
      state = State::ExpectValue;
      break;
    case State::ExpectEnd:
      EXPECT("}");
      state = State::Ended;
      break;
    case State::Ended:
      assert(false && "object loop should have ended");
      break;
    }
  }

  return object;
}

std::shared_ptr<String> ParseString(ParseResult* result, Parser* parser)
{
  enum class State { ExpectStart, AnyCharacter, ExpectSlash, ExpectEnd, Ended };
  State state = State::ExpectStart;
  std::ostringstream ss;

  while(state != State::Ended)
  {
    switch(state)
    {
      case State::ExpectStart:
        EXPECT("\"");
        if(parser->Peek()=='\"')
        {
          state = State::ExpectEnd;
        }
        else if(parser->Peek()=='\\')
        {
          state = State::ExpectSlash;
        }
        else
        {
          state = State::AnyCharacter;
        }
        break;
      case State::AnyCharacter:
      {
        char c = parser->Read();
        if(c == 0 )
        {
          AddError(result, parser, Error::Type::InvalidCharacter, "Unexpected EOF in string");
          return nullptr;
        }
        ss << c;
        if(parser->Peek() == '\\')
        {
         state = State::ExpectSlash;
        }
        else if(parser->Peek() == '\"')
        {
         state = State::ExpectEnd;
        }
      } 
      break;
      case State::ExpectSlash:
      EXPECT("\\");
#define ESCAPE(c, r) if(parser->Peek() == c) { ss << r; }
      ESCAPE('\"', '\"')
      else ESCAPE('\\', '\\')
      else ESCAPE('/', '/')
      else ESCAPE('b', '\b')
      else ESCAPE('f', '\f')
      else ESCAPE('n', '\n')
      else ESCAPE('r', '\r')
      else ESCAPE('t', '\t')
      else if(parser->Peek() == 'u'
          && IsHex(parser->Peek(1))
          && IsHex(parser->Peek(2))
          && IsHex(parser->Peek(3))
          && IsHex(parser->Peek(4))
          )
      {
        parser->Read(); // u
        parser->Read(); // hex1
        parser->Read(); // hex2
        parser->Read(); // hex3
        parser->Read(); // hex4
        AddError(result, parser, Error::Type::UnknownError, "hex not currently handled");
        return nullptr;
      }
      else
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid escape sequence");
        return nullptr;
      }
      if(parser->Peek() == '\"')
      {
        state = State::ExpectEnd;
      }
      else
      {
        state = State::AnyCharacter;
      }
      break;
    case State::ExpectEnd:
      EXPECT("\"");
      state = State::Ended;
      break;
    case State::Ended:
      assert(false && "object loop should have ended");
      break;
    }
  }

  return std::make_shared<String>(ss.str());
}

std::shared_ptr<Number> ParseNumber(ParseResult* result, Parser* parser)
{
  enum class State { ExpectDash, Expect19, ExpectDigit1, Expect0, ExpectDot, ExpectDigit2, ExpectE, ExpectDigit3, Ended };
  State state = State::Expect0;

  std::ostringstream o;

  if (parser->Peek() == '-')
  {
    state = State::ExpectDash;
  }
  else if (IsDigit19(parser->Peek()) )
  {
    state = State::Expect19;
  }
  else if (parser->Peek() == '0')
  {
    state = State::Expect0;
  }
  else
  {
    AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character");
    return nullptr;
  }

  while (state != State::Ended)
  {
    switch (state)
    {
    case State::ExpectDash:
      EXPECT("-");
      o << '-';
      if (parser->Peek() == '0')
      {
        state = State::Expect0;
      }
      else if (IsDigit19(parser->Peek()))
      {
        state = State::Expect19;
      }
      else
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character");
      }
      break;
    case State::Expect19:
    {
      const char c = parser->Read();
      if (!IsDigit19(c))
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character, expected 1-9");
        return nullptr;
      }
      o << c;

      if (IsDigit09(parser->Peek()))
      {
        state = State::ExpectDigit1;
      }
      else if (parser->Peek() == '.')
      {
        state = State::ExpectDot;
      }
      else if (parser->Peek() == 'e' || parser->Peek() == 'E')
      {
        state = State::ExpectE;
      }
      else
      {
        state = State::Ended;
      }
    }
      break;
    case State::ExpectDigit1:
    {
      const char c = parser->Read();
      if (!IsDigit09(c))
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character, expected 0-9");
        return nullptr;
      }
      o << c;

      if (IsDigit09(parser->Peek()))
      {
        // no need to do anything here, just keep looping
      }
      else if (parser->Peek() == '.')
      {
        state = State::ExpectDot;
      }
      else
      {
        state = State::Ended;
      }
    }
      break;
    case State::Expect0:
      EXPECT("0");
      o << '0';
      if (parser->Peek() == '.')
      {
        state = State::ExpectDot;
      }
      else if (parser->Peek() == 'e' || parser->Peek() == 'E')
      {
        state = State::ExpectE;
      }
      else
      {
        state = State::Ended;
      }
      break;
    case State::ExpectDot:
      EXPECT(".");
      o << '.';
      if (IsDigit09(parser->Peek()))
      {
        state = State::ExpectDigit2;
      }
      else
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character, expected 1-9");
        return nullptr;
      }
      break;
    case State::ExpectDigit2:
    {
      const char c = parser->Read();
      if (!IsDigit09(c))
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character, expected 0-9");
        return nullptr;
      }
      o << c;
      if (IsDigit09(parser->Peek()))
      {
        // do nothing
      }
      else if (parser->Peek() == 'e' || parser->Peek() == 'E')
      {
        state = State::ExpectE;
      }
      else
      {
        state = State::Ended;
      }
    }
      break;
    case State::ExpectE:
    {
      char c = parser->Read();
      if (c != 'e' && c != 'E')
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Not E");
        return nullptr;
      }
      o << c;

      if (parser->Peek() == '+' || parser->Peek() == '-')
      {
        char s = parser->Read();
        o << s;
      }
      state = State::ExpectDigit3;
    }
    break;
    case State::ExpectDigit3:
    {
      char d = parser->Read();
      if (!IsDigit09(d))
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Not a digit");
        return nullptr;
      }

      if (!IsDigit09(parser->Peek()) )
      {
        state = State::Ended;
      }
    }
      break;
    case State::Ended:
      assert(false && "number loop should have ended");
      break;
    }
  }

  // todo: is this the correct way to parse?
  std::istringstream in(o.str());
  double d;
  in >> d;
  return std::make_shared<Number>(d);
}

bool IsValidFirstDigit(char c)
{
  if (c == '-') return true;
  if (c == '0') return true;
  if (IsDigit19(c)) return true;
  return false;
}

ParseResult Parse(Parser* parser)
{
  ParseResult res;
  SkipSpaces(parser);

  if (parser->Peek() == '[')
  {
    res.value = ParseArray(&res, parser);
    return res;
  }
  else if(parser->Peek() == '{')
  {
    res.value = ParseObject(&res, parser);
    return res;
  }
  else
  {
    AddError(&res, parser, Error::Type::InvalidCharacter, "Invalid character");
    return res;
  }
}

ParseResult Parse(const std::string& str)
{
  Parser parser{ str };
  return Parse(&parser);
}

#endif // JSONI_IMPLEMENTATION

#endif // JSONI_H

