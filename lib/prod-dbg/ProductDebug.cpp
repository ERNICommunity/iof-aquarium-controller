/*
 * ProductDebug.cpp
 *
 *  Created on: 14.06.2016
 *      Author: nid
 */

#include "ProductDebug.h"

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include <ThingSpeak.h>
#include <Timer.h>
#include <SerialCommand.h>
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgCliCommand.h>
#include <DbgTraceContext.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <DbgPrintConsole.h>
#include <DbgTraceOut.h>

#ifdef ESP8266
extern "C"
{
  #include "user_interface.h"
}
#else
#include <RamUtils.h>
#endif

//-----------------------------------------------------------------------------
// WiFi Commands
//-----------------------------------------------------------------------------
class DbgCli_Cmd_WifiMac : public DbgCli_Command
{
public:
  DbgCli_Cmd_WifiMac(DbgCli_Topic* wifiTopic)
  : DbgCli_Command(wifiTopic, "mac", "Print MAC Address.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    Serial.println();
    Serial.print("Wifi MAC: ");
    Serial.println(WiFi.macAddress().c_str());
    Serial.println();
  }
};

class DbgCli_Cmd_WifiNets : public DbgCli_Command
{
public:
  DbgCli_Cmd_WifiNets(DbgCli_Topic* wifiTopic)
  : DbgCli_Command(wifiTopic, "nets", "Print nearby networks.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    bool bailOut = false;

    // scan for nearby networks:
    Serial.println();
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
      Serial.println("Couldn't get a wifi connection");
      bool bailOut = true;
    }

    if (!bailOut)
    {
      // print the list of networks seen:
      Serial.print("number of available networks:");
      Serial.println(numSsid);

      // print the network number and name for each network found:
      for (int thisNet = 0; thisNet < numSsid; thisNet++)
      {
        Serial.print(thisNet);
        Serial.print(") ");
        Serial.print(WiFi.SSID(thisNet));
        Serial.print("\tSignal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.print("\tEncryption: ");
        printEncryptionType(WiFi.encryptionType(thisNet));
      }
    }
    Serial.println();
  }
private:
  void printEncryptionType(int thisType)
  {
    // read the encryption type and print out the name:
    switch (thisType) {
      case ENC_TYPE_WEP:
        Serial.println("WEP");
        break;
      case ENC_TYPE_TKIP:
        Serial.println("WPA");
        break;
      case ENC_TYPE_CCMP:
        Serial.println("WPA2");
        break;
      case ENC_TYPE_NONE:
        Serial.println("None");
        break;
      case ENC_TYPE_AUTO:
        Serial.println("Auto");
        break;
      default:
        Serial.println("Unknown");
        break;
    }
  }
};

class DbgCli_Cmd_WifiStat : public DbgCli_Command
{
public:
  DbgCli_Cmd_WifiStat(DbgCli_Topic* wifiTopic)
  : DbgCli_Command(wifiTopic, "stat", "Show WiFi connection status.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    wl_status_t wlStatus = WiFi.status();
    Serial.println();
    Serial.println(wlStatus == WL_NO_SHIELD       ? "NO_SHIELD      " :
                   wlStatus == WL_IDLE_STATUS     ? "IDLE_STATUS    " :
                   wlStatus == WL_NO_SSID_AVAIL   ? "NO_SSID_AVAIL  " :
                   wlStatus == WL_SCAN_COMPLETED  ? "SCAN_COMPLETED " :
                   wlStatus == WL_CONNECTED       ? "CONNECTED      " :
                   wlStatus == WL_CONNECT_FAILED  ? "CONNECT_FAILED " :
                   wlStatus == WL_CONNECTION_LOST ? "CONNECTION_LOST" :
                   wlStatus == WL_DISCONNECTED    ? "DISCONNECTED   " : "UNKNOWN");
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.println();
  }
};

class DbgCli_Cmd_WifiDis : public DbgCli_Command
{
public:
  DbgCli_Cmd_WifiDis(DbgCli_Topic* wifiTopic)
  : DbgCli_Command(wifiTopic, "dis", "Disconnect WiFi.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    Serial.println();
    if (argc - idxToFirstArgToHandle > 0)
    {
      printUsage();
    }
    else
    {
      const bool DO_NOT_SET_wifioff = false;
      WiFi.disconnect(DO_NOT_SET_wifioff);
      Serial.println("WiFi is disconnected now.");
    }
    Serial.println();
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg wifi dis");
  }
};

class DbgCli_Cmd_WifiCon : public DbgCli_Command
{
public:
  DbgCli_Cmd_WifiCon(DbgCli_Topic* wifiTopic)
  : DbgCli_Command(wifiTopic, "con", "Connect to WiFi.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    Serial.println();
   if (argc - idxToFirstArgToHandle != 2)
    {
      printUsage();
    }
    else
    {
      WiFi.begin(args[idxToFirstArgToHandle], args[idxToFirstArgToHandle+1]);
      Serial.println("WiFi is connecting now.");
    }
   Serial.println();
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg wifi con <SSID> <passwd>");
  }
};

//-----------------------------------------------------------------------------
// ThingSpeak Commands
//-----------------------------------------------------------------------------
class DbgCli_Cmd_ThingSpeakSetField : public DbgCli_Command
{
public:
  DbgCli_Cmd_ThingSpeakSetField(DbgCli_Topic* thingSpeakTopic)
  : DbgCli_Command(thingSpeakTopic, "set", "Set ThingSpeak field value.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle >= 1)
    {
      unsigned int field = 1;
      if (argc - idxToFirstArgToHandle == 2)
      {
        field = atoi(args[idxToFirstArgToHandle+1]);
      }
      Serial.print("Field: ");
      Serial.print(field);
      Serial.print(" - Value: ");
      Serial.println(static_cast<float>(atof(args[idxToFirstArgToHandle])), 2);
      ThingSpeak.setField(field, static_cast<float>(atof(args[idxToFirstArgToHandle])));
    }
    else
    {
      printUsage();
    }
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg thgspk set <value> [field]");
    Serial.println("       field: 1..8, default: 1");
  }
};

unsigned long thingSpeakChannelId = 0;

class DbgCli_Cmd_ThingSpeakChID : public DbgCli_Command
{
public:
  DbgCli_Cmd_ThingSpeakChID(DbgCli_Topic* thingSpeakTopic)
  : DbgCli_Command(thingSpeakTopic, "chid", "Set ThingSpeak Channel ID.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle != 1)
    {
      printUsage();
    }
    else
    {
      thingSpeakChannelId = atoi(args[idxToFirstArgToHandle]);
      Serial.print("ThingSpeak write fields, Channel Id: ");
      Serial.println(thingSpeakChannelId);
    }
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg thgspk chid <channelId>");
  }
};

char thingSpeakAPIKey[16+1];

class DbgCli_Cmd_ThingSpeakAPIKey : public DbgCli_Command
{
public:
  DbgCli_Cmd_ThingSpeakAPIKey(DbgCli_Topic* thingSpeakTopic)
  : DbgCli_Command(thingSpeakTopic, "key", "Set ThingSpeak API key.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle != 1)
    {
      printUsage();
    }
    else
    {
      strncpy(thingSpeakAPIKey, args[idxToFirstArgToHandle], 16);
      Serial.print("ThingSpeak write fields, API Key: ");
      Serial.println(thingSpeakAPIKey);
    }
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg thgspk key <APIKey>");
  }
};


class DbgCli_Cmd_ThingSpeakWriteFields : public DbgCli_Command
{
public:
  DbgCli_Cmd_ThingSpeakWriteFields(DbgCli_Topic* thingSpeakTopic)
  : DbgCli_Command(thingSpeakTopic, "wr", "Write ThingSpeak fields.")
  { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc - idxToFirstArgToHandle != 0)
    {
      printUsage();
    }
    else
    {
      Serial.print("ThingSpeak write fields, Channel Id: ");
      Serial.print(thingSpeakChannelId);
      Serial.print(", API Key: ");
      Serial.println(thingSpeakAPIKey);
      int status = ThingSpeak.writeFields(thingSpeakChannelId, thingSpeakAPIKey);
      Serial.print("ThingSpeak write done, result: ");
      Serial.println(status ==  200 ? "OK / Success                                                                           " :
                     status ==  404 ? "Incorrect API key (or invalid ThingSpeak server address)                               " :
                     status == -101 ? "Value is out of range or string is too long (> 255 characters)                         " :
                     status == -201 ? "Invalid field number specified                                                         " :
                     status == -210 ? "setField() was not called before writeFields()                                         " :
                     status == -301 ? "Failed to connect to ThingSpeak                                                        " :
                     status == -302 ? "Unexpected failure during write to ThingSpeak                                          " :
                     status == -303 ? "Unable to parse response                                                               " :
                     status == -304 ? "Timeout waiting for server to respond                                                  " :
                     status == -401 ? "Point was not inserted (most probable cause is the rate limit of once every 15 seconds)" : "UNKNOWN");
    }
  }

  void printUsage()
  {
    Serial.println(getHelpText());
    Serial.println("Usage: dbg thgspk wr");
  }
};

//-----------------------------------------------------------------------------
// Free Heap Logger
//-----------------------------------------------------------------------------
const unsigned long c_freeHeapLogIntervalMillis = 10000;

class FreeHeapLogTimerAdapter : public TimerAdapter
{
private:
  DbgTrace_Port* m_trPort;
public:
  FreeHeapLogTimerAdapter()
  : m_trPort(new DbgTrace_Port("heap", DbgTrace_Level::info))
  { }

  void timeExpired()
  {
#ifdef ESP8266
    TR_PRINT_LONG(m_trPort, DbgTrace_Level::debug, system_get_free_heap_size());
#else
    TR_PRINT_LONG(m_trPort, DbgTrace_Level::debug, RamUtils::getFreeRam());
#endif
  }
};

//-----------------------------------------------------------------------------

extern SerialCommand* sCmd;

void setupDebugEnv()
{
  //-----------------------------------------------------------------------------
  // Serial Command Object for Debug CLI
  //-----------------------------------------------------------------------------
  Serial.begin(115200);
  sCmd = new SerialCommand();
  DbgCli_Node::AssignRootNode(new DbgCli_Topic(0, "dbg", "Wiring Controller Debug CLI Root Node."));

  // Setup callbacks for SerialCommand commands
  if (0 != sCmd)
  {
    sCmd->addCommand("dbg", dbgCliExecute);
    sCmd->addCommand("hello", sayHello);        // Echos the string argument back
    sCmd->setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  }

  //-----------------------------------------------------------------------------
  // WiFi Commands
  //-----------------------------------------------------------------------------
#ifdef ESP8266
  DbgCli_Topic* wifiTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "wifi", "WiFi debug commands");
  new DbgCli_Cmd_WifiMac(wifiTopic);
  new DbgCli_Cmd_WifiNets(wifiTopic);
  new DbgCli_Cmd_WifiStat(wifiTopic);
  new DbgCli_Cmd_WifiDis(wifiTopic);
  new DbgCli_Cmd_WifiCon(wifiTopic);
#endif

  //-----------------------------------------------------------------------------
  // ThingSpeak Commands
  //-----------------------------------------------------------------------------
#ifdef ESP8266
  DbgCli_Topic* thingSpeakTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "thgspk", "ThingSpeak debug commands");
  new DbgCli_Cmd_ThingSpeakSetField(thingSpeakTopic);
  new DbgCli_Cmd_ThingSpeakChID(thingSpeakTopic);
  new DbgCli_Cmd_ThingSpeakAPIKey(thingSpeakTopic);
  new DbgCli_Cmd_ThingSpeakWriteFields(thingSpeakTopic);
#endif

  //---------------------------------------------------------------------------
  // Debug Trace
  //---------------------------------------------------------------------------
  new DbgTrace_Context(new DbgCli_Topic(DbgCli_Node::RootNode(), "tr", "Modify debug trace"));
  new DbgTrace_Out(DbgTrace_Context::getContext(), "trConOut", new DbgPrint_Console());

  //-----------------------------------------------------------------------------
  // Free Heap Logger
  //-----------------------------------------------------------------------------
  new Timer(new FreeHeapLogTimerAdapter(), Timer::IS_RECURRING, c_freeHeapLogIntervalMillis);

  Serial.println();
  Serial.println("---------------------------------------------");
  Serial.println("Hello from Wiring Controller!");
  Serial.println("---------------------------------------------");
  Serial.println();
}

void dbgCliExecute()
{
  if ((0 != sCmd) && (0 != DbgCli_Node::RootNode()))
  {
    const unsigned int firstArgToHandle = 1;
    const unsigned int maxArgCnt = 10;
    char* args[maxArgCnt];
    char* arg = const_cast<char*>("dbg");
    unsigned int arg_cnt = 0;
    while ((maxArgCnt > arg_cnt) && (0 != arg))
    {
      args[arg_cnt] = arg;
      arg = sCmd->next();
      arg_cnt++;
    }
    DbgCli_Node::RootNode()->execute(static_cast<unsigned int>(arg_cnt), const_cast<const char**>(args), firstArgToHandle);
  }
}

void sayHello()
{
  char *arg;
  if (0 != sCmd)
  {
    arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  }
  else
  {
    arg = const_cast<char*>("");;
  }
  if (arg != NULL)         // As long as it exists, take it
  {
    Serial.print("Hello ");
    Serial.println(arg);
  }
  else
  {
    Serial.println("Hello, whoever you are");
  }
}

// This is the default handler, and gets called when no other command matches.
void unrecognized(const char *command)
{
  Serial.println("What?");
}
