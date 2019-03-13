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
  Location() : line(-1), column(-1) {}
  explicit Location(int l) : line(l), column(-1) {}
  Location(int l, int c) : line(l), column(c) {}

  int line;
  int column;
};

std::ostream& operator<<(std::ostream& s, const Location& location)
{
  s << "(" << location.line << ", " << location.column << ")";
  return s;
}

struct Visitor
{
  virtual void VisitObject (Object* object ) = 0;
  virtual void VisitArray  (Array * array  ) = 0;
  virtual void VisitString (String* string ) = 0;
  virtual void VisitNumber (Number* number ) = 0;
  virtual void VisitBool   (Bool  * boolean) = 0;
  virtual void VisitNull   (Null  * null   ) = 0;
  virtual void VisitInt(Int   * integer);
};

struct Value
{
  Location location;

  virtual ~Value() {}

  virtual void Visit(Visitor* visitor) = 0;

  virtual Object* AsObject() { return nullptr; }
  virtual Array * AsArray () { return nullptr; }
  virtual String* AsString() { return nullptr; }
  virtual Number* AsNumber() { return nullptr; }
  virtual Bool  * AsBool  () { return nullptr; }
  virtual Null  * AsNull  () { return nullptr; }
  virtual Int   * AsInt() { return nullptr; }
};

struct Object : public Value
{
  std::map<std::string, std::shared_ptr<Value>> object;

  void Visit(Visitor* visitor) override { visitor->VisitObject(this); }

  Object* AsObject() override { return this; }
};

struct Array : public Value
{
  std::vector<std::shared_ptr<Value>> array;

  void Visit(Visitor* visitor) override { visitor->VisitArray(this); }

  Array* AsArray() override { return this; }
};

struct String : public Value
{
  std::string string;

  void Visit(Visitor* visitor) override { visitor->VisitString(this); }

  String* AsString() override { return this; }

  explicit String(const std::string& s="") : string(s) {}
};

struct Number : public Value
{
  double number;

  void Visit(Visitor* visitor) override { visitor->VisitNumber(this); }

  Number* AsNumber() override { return this; }

  explicit Number(double d) : number(d) {}
};

struct Bool : public Value
{
  bool boolean;

  void Visit(Visitor* visitor) override { visitor->VisitBool(this); }

  Bool* AsBool() override { return this; }

  explicit Bool(bool b) : boolean(b) {}
};

struct Null : public Value
{
  void Visit(Visitor* visitor) override { visitor->VisitNull(this); }

  Null* AsNull() override { return this; }
};

struct Int : public Number
{
  int integer;

  void Visit(Visitor* visitor) override { visitor->VisitInt(this); }

  Int* AsInt() override { return this; }

  explicit Int(int i) : Number(i), integer(i) { }
};

void Visitor::VisitInt(Int* integer) { VisitNumber(integer); }

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

  Error(Type t, const std::string& m, const Location& l=Location()) : type(t), message(m), location(l) {}
};

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

// todo: this structure seems weird, make it into something that feels less weird
struct ParseResult
{
  // contains errors if parsing failed
  std::vector<Error> errors;

  // one of these are non-null if parsing didn't fail
  std::shared_ptr<Object> object;
  std::shared_ptr<Array> array;

  bool HasError() const
  {
    return object == nullptr && array == nullptr;
  }

  operator bool() const
  {
    return !HasError();
  }
};

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

ParseResult Parse(const std::string& str);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation and detail..

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
  result->array = nullptr;
  result->object = nullptr;
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
  enum class State { ExpectDash, Expect19, ExpectDigit1, Expect0, ExpectDot, ExpectDigit2, ExpectE, Ended };
  State state = State::Expect0;

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

      if (IsDigit09(parser->Peek()))
      {
        state = State::ExpectDigit1;
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
    case State::ExpectDigit1:
    {
      const char c = parser->Read();
      if (!IsDigit09(c))
      {
        AddError(result, parser, Error::Type::InvalidCharacter, "Invalid character, expected 0-9");
        return nullptr;
      }

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
      AddError(result, parser, Error::Type::InvalidCharacter, "Not implemented exponent numbers yet");
      return nullptr;
      break;
    case State::Ended:
      assert(false && "number loop should have ended");
      break;
    }
  }

  // todo: actually parse number...
  return std::make_shared<Number>(42.0);
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
    ParseArray(&res, parser);
    return res;
  }
  else if(parser->Peek() == '{')
  {
    ParseObject(&res, parser);
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

#endif // JSONI_H

