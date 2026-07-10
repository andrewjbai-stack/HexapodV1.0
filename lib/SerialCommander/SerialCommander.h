#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include <Arduino.h>
#include <functional>

// Holds the parsed arguments for a single command invocation.
// Arguments are the comma-separated values found after the command name.
class CommandArgs {
  public:
    CommandArgs();

    void parse(const String& argString);

    int count() const;
    long getInt(int index, long defaultValue = 0) const;
    float getFloat(int index, float defaultValue = 0.0f) const;
    String getString(int index, const String& defaultValue = "") const;
    String raw() const;

  private:
    static const int MAX_ARGS = 8;
    String _args[MAX_ARGS];
    int _count;
    String _raw;
};

// Callback signature used for every registered command.
typedef std::function<void(CommandArgs&)> CommandCallback;

// SerialCommander reads lines from a Stream (Serial, Serial1, BluetoothSerial, etc.)
// and dispatches them to registered command callbacks.
//
// Expected line format:   COMMAND:arg1,arg2,arg3
// Examples:               STRAFE:10,20,1
//                         STOP:
//                         LED:1
//
// Usage:
//   SerialCommander commander;
//
//   void setup() {
//     Serial.begin(115200);
//     commander.registerCommand("STRAFE", onStrafe);
//     commander.registerCommand("STOP", onStop);
//   }
//
//   void loop() {
//     commander.handle();
//   }
class SerialCommander {
  public:
    // stream: which Stream to read from (defaults to Serial)
    // terminator: character marking end of a command line (defaults to '\n')
    SerialCommander(Stream& stream = Serial, char terminator = '\n');

    // Call once per loop() iteration. Reads any available bytes,
    // and dispatches complete lines to their matching command callback.
    void handle();

    // Register a new command. Names are case-insensitive.
    // Returns false if the command table is full (see MAX_COMMANDS).
    bool registerCommand(const String& name, CommandCallback callback);

    // Optional: called when a received line doesn't match any registered command.
    void setUnknownCommandCallback(CommandCallback callback);

    // Optional: echo every received character back out (useful for debugging over serial).
    void setEchoEnabled(bool enabled);

  private:
    static const int MAX_COMMANDS = 20;

    struct CommandEntry {
      String name;
      CommandCallback callback;
    };

    Stream& _stream;
    char _terminator;
    String _buffer;
    bool _echo;

    CommandEntry _commands[MAX_COMMANDS];
    int _commandCount;
    CommandCallback _unknownCallback;

    void processLine(String line);
};

#endif // SERIAL_COMMANDER_H
