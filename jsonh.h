#ifndef JSONH_H
#define JSONH_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <sstream>

#include <cassert>

#include <cstdint>

// defines
using tint = int64_t;
using tloc = size_t;
using tnum = double;

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
  Location(tloc l, tloc c);

  tloc line;
  tloc column;
};

std::ostream& operator<<(std::ostream& s, const Location& location);

struct Visitor
{
  // will not recurse, if you want to visit the children, you have to keep calling Visit
  virtual void VisitObject (Object* object ) = 0;
  virtual void VisitArray  (Array * array  ) = 0;

  virtual void VisitString (String* string ) = 0;
  virtual void VisitNumber (Number* number ) = 0;
  virtual void VisitBool   (Bool  * boolean) = 0;
  virtual void VisitNull   (Null  * null   ) = 0;
  virtual void VisitInt    (Int   * integer) = 0;
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

  // only exact matches
  virtual Object* AsObject();
  virtual Array * AsArray ();
  virtual String* AsString();
  virtual Number* AsNumber();
  virtual Bool  * AsBool  ();
  virtual Null  * AsNull  ();
  virtual Int   * AsInt   ();
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
  tnum number;

  void Visit(Visitor* visitor) override;

  Number* AsNumber() override;

  explicit Number(tnum d);
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

struct Int : public Value
{
  tint integer;

  void Visit(Visitor* visitor) override;

  Int* AsInt() override;

  explicit Int(tint i);
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

namespace ParseFlags
{
  enum Type
  {
    None = 0,

    Json = None
  };
}

std::ostream& operator<<(std::ostream& s, const ParseResult& result);

ParseResult Parse(const std::string& str, ParseFlags::Type flags);

#ifdef JSONH_IMPLEMENTATION

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation detail..

Location::Location() : line(0), column(0) {}
Location::Location(tloc l, tloc c) : line(l), column(c) {}

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
Int   * Value::AsInt   () { return nullptr; }

struct PrettyPrintVisitor : public Visitor
{
  PrettyPrint settings;
  std::ostream* stream;

  void StreamString(const std::string& str)
  {
    *stream << '\"';
    for (char c : str)
    {
      switch(c)
      {
      case '\"': case '\\':
        *stream << "\\" << c;
        break;
      case '\b':
        *stream << "\\b";
        break;
      case '\f':
        *stream << "\\f";
        break;
      case '\n':
        *stream << "\\n";
        break;
      case '\r':
        *stream << "\\r";
        break;
      case '\t':
        *stream << "\\t";
        break;
      default:
        *stream << c;
        break;
      }
    }
    *stream << '\"';
  }

  void VisitObject(Object* object) override
  {
    bool first = true;
    *stream << '{';
    for (auto o : object->object)
    {
      if (first) { first = false; }
      else { *stream << ','; }
      StreamString(o.first);
      *stream << ':';
      o.second->Visit(this);
    }
    *stream << '}';
  }
  void VisitArray(Array * array) override
  {
    bool first = true;
    *stream << '[';
    for (auto o : array->array)
    {
      if (first) { first = false; }
      else       { *stream << ','; }
      o->Visit(this);
    }
    *stream << ']';
  }

  void VisitString(String* string) override
  {
    StreamString(string->string);
  }
  void VisitNumber(Number* number) override
  {
    // can't really detect if it is -0 or 0, should we? does it have a special value?
    // https://stackoverflow.com/questions/45795397/behaviour-of-negative-zero-0-0-in-comparison-with-positive-zero-0-0/45795465
    if(number->number == 0 ) *stream << "0.0";
    else *stream << number->number;
  }
  void VisitBool(Bool  * boolean) override
  {
    *stream << (boolean->boolean ? "true" : "false");
  }
  void VisitNull(Null*) override
  {
    *stream << "null";
  }
  void VisitInt(Int* integer) override
  {
    *stream << integer->integer;
  }
};

std::string ToString(Value* value, const PrettyPrint& pp)
{
  std::ostringstream ss;
  PrettyPrintVisitor vis;
  vis.settings = pp;
  vis.stream = &ss;
  value->Visit(&vis);
  return ss.str();
}

void Object::Visit(Visitor* visitor) { visitor->VisitObject(this); }
Object* Object::AsObject() { return this; }

void Array::Visit(Visitor* visitor) { visitor->VisitArray(this); }
Array* Array::AsArray() { return this; }

void String::Visit(Visitor* visitor) { visitor->VisitString(this); }
String* String::AsString() { return this; }
String::String(const std::string& s) : string(s) {}

void Number::Visit(Visitor* visitor) { visitor->VisitNumber(this); }
Number* Number::AsNumber() { return this; }
Number::Number(tnum d) : number(d) {}

void Bool::Visit(Visitor* visitor) { visitor->VisitBool(this); }
Bool* Bool::AsBool() { return this; }
Bool::Bool(bool b) : boolean(b) {}

void Null::Visit(Visitor* visitor) { visitor->VisitNull(this); }
Null* Null::AsNull() { return this; }

void Int::Visit(Visitor* visitor) { visitor->VisitInt(this); }
Int* Int::AsInt() { return this; }
Int::Int(tint i) : integer(i) { }


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
    s << ToString(result.value.get(), PrettyPrint::Compact());
  }
  return s;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here comes the parser

struct Parser;

struct Parser
{
  std::string str;
  tloc index = 0;

  explicit Parser(const std::string& s) : str(s) {}

  char Peek(tloc advance = 0) const
  {
    const tloc i = index + advance;
    if (index >= str.size()) return 0;
    return str[i];
  }

  bool HasMoreChar() const
  {
    return index < str.size();
  }

  tloc line = 1;
  tloc column = 0;

  Location GetLocation() const
  {
    return {line, column};
  }

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

void AppendChar(std::ostream& s, char c)
{
  switch(c)
  {
    case '\n':
      s << "<newline>";
      break;
    case '\r':
      s << "<linefeed>";
      break;
    case 0:
      s << "<EOF>";
      break;
    case '\t':
      s << "<tab>";
      break;
    case ' ':
      s << "<space>";
      break;
    default:
      s << c << "(" << static_cast<int>(static_cast<unsigned char>(c)) << ")";
      break;
  }
}

#define EXPECT(error_type, expected_char)\
  do\
  {\
    char c = parser->Read();\
    if( c != expected_char )\
    {\
      std::stringstream expect_ss;\
      expect_ss << "Expected character " << expected_char << " but found ";\
      AppendChar(expect_ss, c);\
      AddError(result, parser, c?error_type:Error::Type::UnexpectedEof, expect_ss.str());\
      return nullptr;\
    }\
  } while(false)

void AddError(ParseResult* result, Parser* parser, Error::Type type, const std::string& err)
{
  result->errors.push_back(Error{type, err, Location{parser->line, parser->column} });
  // todo: assert here instead of assigning, since the value shouldn't be set anyway...
  result->value = nullptr;
}

void AddNote(ParseResult* result, const Location& loc, const std::string& note)
{
  result->errors.push_back(Error{Error::Type::Note, note, loc});
}

std::shared_ptr<Value > ParseValue  (ParseResult* result, Parser* parser);
std::shared_ptr<Object> ParseObject (ParseResult* result, Parser* parser);
std::shared_ptr<Array > ParseArray  (ParseResult* result, Parser* parser);
std::shared_ptr<String> ParseString (ParseResult* result, Parser* parser);
std::shared_ptr<Value>  ParseNumber (ParseResult* result, Parser* parser);

bool IsDigit(char c)
{
  return c >= '0' && c <= '9';
}

bool IsHex(char c)
{
  return IsDigit(c)
    || (c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    ;
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
    auto ret = std::make_shared<Bool>(true);
    ret->location = parser->GetLocation();
    parser->Read(); // t
    parser->Read(); // r
    parser->Read(); // u
    parser->Read(); // e
    SkipSpaces(parser);
    return ret;
  }

  if (
    parser->Peek(0) == 'f' &&
    parser->Peek(1) == 'a' &&
    parser->Peek(2) == 'l' &&
    parser->Peek(3) == 's' &&
    parser->Peek(4) == 'e'
    )
  {
    auto ret = std::make_shared<Bool>(false);
    ret->location = parser->GetLocation();
    parser->Read(); // f
    parser->Read(); // a
    parser->Read(); // l
    parser->Read(); // s
    parser->Read(); // e
    SkipSpaces(parser);
    return ret;
  }

  if (
    parser->Peek(0) == 'n' &&
    parser->Peek(1) == 'u' &&
    parser->Peek(2) == 'l' &&
    parser->Peek(3) == 'l'
    )
  {
    auto ret = std::make_shared<Null>();
    ret->location = parser->GetLocation();
    parser->Read(); // n
    parser->Read(); // u
    parser->Read(); // l
    parser->Read(); // l
    SkipSpaces(parser);
    return ret;
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
  auto array = std::make_shared<Array>();
  array->location = parser->GetLocation();

  EXPECT(Error::Type::InvalidCharacter, '[');
  SkipSpaces(parser);

  const auto start_of_array = parser->GetLocation();

  bool first = true;
  while (parser->HasMoreChar() && parser->Peek() != ']')
  {
    if(first)
    {
      first = false;
    }
    else
    {
      EXPECT(Error::Type::InvalidCharacter, ',');
      SkipSpaces(parser);
    }

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

    const auto next_char = parser->Peek();
    switch(next_char)
    {
      case ':':
        AddError(result, parser, Error::Type::InvalidCharacter, "Found colon instead of comma");
        return nullptr;
      case '}':
        AddError(result, parser, Error::Type::UnclosedArray, "Found }. A square bracket ] closes the array.");
        AddNote(result, start_of_array, "Array started here");
        return nullptr;
    }
  }

  const auto end = parser->Read();
  if(end != ']')
  {
    std::ostringstream ss;
    ss << "Expected end of array but found ";
    AppendChar(ss, end);
    AddError(result, parser, Error::Type::UnclosedArray, ss.str());
    AddNote(result, start_of_array, "Array started here");
    return nullptr;
  }
  return array;
}

std::shared_ptr<Object> ParseObject(ParseResult* result, Parser* parser)
{
  EXPECT(Error::Type::InvalidCharacter, '{');
  auto object = std::make_shared<Object>();
  object->location = parser->GetLocation();
  SkipSpaces(parser);

  bool first = true;
  while (parser->HasMoreChar() && parser->Peek() != '}')
  {
    if(first)
    {
      first = false;
    }
    else
    {
      EXPECT(Error::Type::InvalidCharacter, ',');
      SkipSpaces(parser);
    }

    auto s = ParseString(result, parser);
    if (s == nullptr)
    {
      return nullptr;
    }
    EXPECT(Error::Type::InvalidCharacter, ':');
    SkipSpaces(parser);
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
  }

  EXPECT(Error::Type::InvalidCharacter, '}');
  SkipSpaces(parser);

  return object;
}

char CharToHex(char c)
{
  if(c >= '0' && c<='9')
  {
    return c - '0';
  }
  else if(c >= 'a' && c<='f')
  {
    return 10 + (c - 'a');
  }
  else if(c >= 'A' && c<='F')
  {
    return 10 + (c - 'A');
  }
  else
  {
    assert(false && "Invalid hex code");
    return 0;
  }
}

bool ParseEscapeCode(ParseResult* result, Parser* parser, std::ostringstream& ss)
{
#define ESCAPE(c, r) if(parser->Peek() == c) { parser->Read(); ss << r; }
  ESCAPE('\"', '\"')
  else ESCAPE('\\', '\\')
  else ESCAPE('/', '/')
  else ESCAPE('b', '\b')
  else ESCAPE('f', '\f')
  else ESCAPE('n', '\n')
  else ESCAPE('r', '\r')
  else ESCAPE('t', '\t')
#undef ESCAPE
  else if(parser->Peek() == 'u'
      && IsHex(parser->Peek(1))
      && IsHex(parser->Peek(2))
      && IsHex(parser->Peek(3))
      && IsHex(parser->Peek(4))
      )
  {
    parser->Read(); // u
    const auto hex1 = CharToHex(parser->Read()); // hex1
    const auto hex2 = CharToHex(parser->Read()); // hex2
    const auto hex3 = CharToHex(parser->Read()); // hex3
    const auto hex4 = CharToHex(parser->Read()); // hex4
    const char val = (hex1 << 12) | (hex2 << 8) | (hex3 << 4) | hex4;
    ss << val;
  }
  else
  {
    const char escape_char = parser->Peek();
    std::ostringstream ess;
    ess << "Illegal escape sequence: ";
    AppendChar(ess, escape_char);
    AddError(result, parser, Error::Type::IllegalEscape, ess.str());
    return false;
  }

  return true;
}

std::shared_ptr<String> ParseString(ParseResult* result, Parser* parser)
{
  const auto loc = parser->GetLocation();
  EXPECT(Error::Type::InvalidCharacter, '\"');

  std::ostringstream string_buffer;

  while(parser->Peek() != '\"')
  {
    const char c = parser->Read();
    if(c == 0 )
    {
      AddError(result, parser, Error::Type::InvalidCharacter, "Unexpected EOF in string");
      return nullptr;
    }
    else if(c == '\\')
    {
      if(!ParseEscapeCode(result, parser, string_buffer))
      {
        return nullptr;
      }
    }
    // is this correct? checker fail25 and fail27 seems to think so
    // but json.org says to allow any unicode character
    else if(c == '\n' || c=='\t')
    {
      std::ostringstream ss;
      ss << "the ";
      AppendChar(ss, c);
      ss << " character must be escaped";
      AddError(result, parser, Error::Type::InvalidCharacterInString, ss.str());
      return nullptr;
    }
    else
    {
      string_buffer << c;
    }
  }
  EXPECT(Error::Type::InvalidCharacter, '\"');
  SkipSpaces(parser);

  auto ret = std::make_shared<String>(string_buffer.str());
  ret->location = loc;
  return ret;
}

std::shared_ptr<Value> ParseNumber(ParseResult* result, Parser* parser)
{
  std::ostringstream o;

  const auto loc = parser->GetLocation();

  if (parser->Peek() == '-')
  {
    o << parser->Read();
  }

  if (parser->Peek() == '0')
  {
    o << parser->Read();
    if(IsDigit(parser->Peek()))
    {
      AddError(result, parser, Error::Type::InvalidNumber, "Numbers can't have leading zeroes");
      return nullptr;
    }
  }
  else
  {
    if(!IsDigit(parser->Peek()))
    {
      AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character as a number");
      return nullptr;
    }
    o << parser->Read();
    while(IsDigit(parser->Peek()))
    {
      o << parser->Read();
    }
  }

  bool is_integer = true;
  
  if(parser->Peek() == '.')
  {
    is_integer = false;
    o << parser->Read();
    if(!IsDigit(parser->Peek()))
    {
      AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character in a fractional number");
      return nullptr;
    }
    o << parser->Read();
    while(IsDigit(parser->Peek()))
    {
      o << parser->Read();
    }
  }

  if(parser->Peek() == 'e' || parser->Peek() == 'E')
  {
    is_integer = false;
    o << parser->Read();
    if(parser->Peek() == '+' || parser->Peek() == '-')
    {
      o << parser->Read();
    }

    if(!IsDigit(parser->Peek()))
    {
      AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character in a exponent");
      return nullptr;
    }
    o << parser->Read();
    while(IsDigit(parser->Peek()))
    {
      o << parser->Read();
    }
  }

  // todo: is this the correct way to parse?
  std::istringstream in(o.str());
  if(is_integer)
  {
    tint d;
    in >> d;
    if(in.fail())
    {
      AddError(result, parser, Error::Type::UnknownError, "Failed to parse integer: " + o.str());
      return nullptr;
    }
    auto ret = std::make_shared<Int>(d);
    ret->location = loc;
    return ret;
  }
  else
  {
    tnum d;
    in >> d;
    // osx sometimes fails here, but parsing looks ok...?
    // if(in.fail())
    // {
    // AddError(result, parser, Error::Type::UnknownError, "Failed to parse number: " + o.str());
    // return nullptr;
    // }
    auto ret = std::make_shared<Number>(d);
    ret->location = loc;
    return ret;
  }
}

bool IsValidFirstDigit(char c)
{
  if (c == '-') return true;
  if (IsDigit(c)) return true;
  return false;
}

ParseResult Parse(Parser* parser)
{
  ParseResult res;
  SkipSpaces(parser);

  if (parser->Peek() == '[')
  {
    res.value = ParseArray(&res, parser);
    SkipSpaces(parser);
    if(res.value.get() != nullptr && parser->HasMoreChar())
    {
      AddError(&res, parser, Error::Type::NotEof, "Expected EOF after array");
    }
    return res;
  }
  else if(parser->Peek() == '{')
  {
    res.value = ParseObject(&res, parser);
    SkipSpaces(parser);
    if(res.value.get() != nullptr && parser->HasMoreChar())
    {
      AddError(&res, parser, Error::Type::NotEof, "Expected EOF after object");
    }
    return res;
  }
  else
  {
    AddError(&res, parser, Error::Type::InvalidCharacter, "Invalid character");
    return res;
  }
}

ParseResult Parse(const std::string& str, ParseFlags::Type)
{
  Parser parser{ str };
  return Parse(&parser);
}

#endif // JSONH_IMPLEMENTATION

#endif // JSONH_H

