#include "SerialCommander.h"

// ---------------- CommandArgs ----------------

CommandArgs::CommandArgs() : _count(0) {}

void CommandArgs::parse(const String& argString) {
  _count = 0;
  _raw = argString;

  if (argString.length() == 0) return;

  int start = 0;
  while (_count < MAX_ARGS) {
    int commaIndex = argString.indexOf(',', start);
    if (commaIndex == -1) {
      _args[_count++] = argString.substring(start);
      break;
    } else {
      _args[_count++] = argString.substring(start, commaIndex);
      start = commaIndex + 1;
    }
  }
}

int CommandArgs::count() const {
  return _count;
}

long CommandArgs::getInt(int index, long defaultValue) const {
  if (index < 0 || index >= _count) return defaultValue;
  return _args[index].toInt();
}

float CommandArgs::getFloat(int index, float defaultValue) const {
  if (index < 0 || index >= _count) return defaultValue;
  return _args[index].toFloat();
}

String CommandArgs::getString(int index, const String& defaultValue) const {
  if (index < 0 || index >= _count) return defaultValue;
  return _args[index];
}

String CommandArgs::raw() const {
  return _raw;
}

// ---------------- SerialCommander ----------------

SerialCommander::SerialCommander(Stream& stream, char terminator)
    : _stream(stream),
      _terminator(terminator),
      _buffer(""),
      _echo(false),
      _commandCount(0),
      _unknownCallback(nullptr) {}

void SerialCommander::handle() {
  while (_stream.available() > 0) {
    char c = _stream.read();

    if (_echo) {
      _stream.write(c);
    }

    if (c == _terminator) {
      if (_buffer.length() > 0) {
        processLine(_buffer);
        _buffer = "";
      }
    } else if (c == '\r') {
      // ignore, so it works whether the sender sends \n or \r\n
    } else {
      _buffer += c;
    }
  }
}

bool SerialCommander::registerCommand(const String& name, CommandCallback callback) {
  if (_commandCount >= MAX_COMMANDS) return false;

  String upperName = name;
  upperName.toUpperCase();
  upperName.trim();

  _commands[_commandCount].name = upperName;
  _commands[_commandCount].callback = callback;
  _commandCount++;
  return true;
}

void SerialCommander::setUnknownCommandCallback(CommandCallback callback) {
  _unknownCallback = callback;
}

void SerialCommander::setEchoEnabled(bool enabled) {
  _echo = enabled;
}

void SerialCommander::processLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  int colonIndex = line.indexOf(':');
  String name;
  String argString;

  if (colonIndex == -1) {
    name = line;
    argString = "";
  } else {
    name = line.substring(0, colonIndex);
    argString = line.substring(colonIndex + 1);
  }

  name.toUpperCase();
  name.trim();

  for (int i = 0; i < _commandCount; i++) {
    if (_commands[i].name == name) {
      CommandArgs args;
      args.parse(argString);
      _commands[i].callback(args);
      return;
    }
  }

  // No match found
  if (_unknownCallback) {
    CommandArgs args;
    args.parse(line);
    _unknownCallback(args);
  } else {
    _stream.print("Unknown command: ");
    _stream.println(name);
  }
}
