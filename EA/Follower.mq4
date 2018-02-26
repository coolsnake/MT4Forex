//+------------------------------------------------------------------+
//|                                         ���ٸ���ϵͳ �ͻ���         |
//|                                                                  |
//|                                                                  |
//+------------------------------------------------------------------+
#property copyright "Zhao Zhe"
#property link      "alex.zhao"
#property show_inputs

#import "EAClient.dll"
int InitNetwork();
int DeInitNetwork();
int ConnectToHost(string HostName);
int CloseConnection(int ClientSock);
int SendAuthMessage(int ClientSock, string user, string key, string MM, int MT4Account);
int SendAuthEAIDMessage(int ClientSock, string user, string key, int EAID);
int ReceiveFromHost(int ClientSock, string symbol, int &cmd[], int &slippage[], double &volume[], double &percent[],
                    double &price[], double &stoploss[], double &takeprofit[], int &operate[], int &ticket[], int &opid[]);
int SendOrderAck(int ClientSock, string symbol, int cmd, int slippage,
                 double volume, double percent, double price, double stoploss, double takeprofit, double profit,
                 int operate, int ticket, int opid);
int SetRecvTimeOut(int ClientSock, int TimeOut);
int SetTCPNoDelay(int ClientSock, int enable);
#import

#include <WinUser32.mqh>
#include <stderror.mqh>

extern string �û��� = "";
extern string ��½���� = "";

extern bool �Զ����� = true;
extern bool ���ֱֲ������� = true;

bool initialized = false;
bool retry_warn = true;
bool setting_error = true;
int socket;
int filehandle;
int retry_login_count = 0;

double LotSize;
double MinLot;
double MaxLot;
double LotStep;


string username = "demo";
string password = "demo";

int prev_last_order;
int prev_close_order;

#define MAX_TRADE_TOOLS 30

string company;
string trade_tools[MAX_TRADE_TOOLS];
double trade_slip[MAX_TRADE_TOOLS];

int init_table()
{
   company = TerminalCompany();

   trade_slip[0] = 0.5;
   trade_slip[1] = 0.5;
   trade_slip[2] = 0.0005;
   trade_slip[3] = 0.05;
   trade_slip[4] = 0.0005;
   trade_slip[5] = 0.0005;
   trade_slip[6] = 0.0005;
   trade_slip[7] = 0.20;
   trade_slip[8] = 0.0020;
   trade_slip[9] = 0.0020;
   trade_slip[10] = 0.20;
   trade_slip[11] = 0.0005;
   trade_slip[12] = 0.0005;
   trade_slip[13] = 0.20;
   trade_slip[14] = 0.0020;
   trade_slip[15] = 0.20;
   trade_slip[16] = 0.20;
   trade_slip[17] = 0.0020;
   trade_slip[18] = 0.0020;
   trade_slip[19] = 0.20;
   trade_slip[20] = 0.50;
   trade_slip[21] = 0.0020;
   trade_slip[22] = 0.0020;
   trade_slip[23] = 0.002;
   trade_slip[24] = 0.002;

   if (company == "Performance Ronnaru Co., Ltd.")
   {
      trade_tools[0] = "XAUUSD";
      trade_tools[1] = "XAGUSD";
      trade_tools[2] = "EURUSD/";
      trade_tools[3] = "USDJPY/";
      trade_tools[4] = "GBPUSD=";
      trade_tools[5] = "USDCHF=";
      trade_tools[6] = "AUDUSD=";
      trade_tools[7] = "EURJPY=";
      trade_tools[8] = "EURGBP=";
      trade_tools[9] = "EURCHF=";
      trade_tools[10] = "GBPJPY=";
      trade_tools[11] = "USDCAD=";
      trade_tools[12] = "NZDUSD=";
      trade_tools[13] = "AUDJPY=";
   }

   if (company == "Asyusyoji Co., Ltd")
   {
      trade_tools[0] = "XAUUSD";
      trade_tools[1] = "XAGUSD";
      trade_tools[2] = "EURUSD/";
      trade_tools[3] = "USDJPY/";
      trade_tools[4] = "GBPUSD=";
      trade_tools[5] = "USDCHF=";
      trade_tools[6] = "AUDUSD=";
      trade_tools[7] = "EURJPY=";
      trade_tools[8] = "EURGBP=";
      trade_tools[9] = "EURCHF=";
      trade_tools[10] = "GBPJPY=";
      trade_tools[11] = "USDCAD=";
      trade_tools[12] = "NZDUSD=";
      trade_tools[13] = "AUDJPY=";
   }

   if (company == "Macro Dealing Advisor")
   {
      trade_tools[0] = "XAU_USD";
      trade_tools[2] = "EURUSD";
      trade_tools[3] = "USDJPY";
      trade_tools[4] = "GBPUSD";
      trade_tools[5] = "USDCHF";
      trade_tools[6] = "AUDUSD";
      trade_tools[7] = "EURJPY";
      trade_tools[8] = "EURGBP";
      trade_tools[9] = "EURCHF";
      trade_tools[10] = "GBPJPY";
      trade_tools[11] = "USDCAD";
      trade_tools[12] = "NZDUSD";
      trade_tools[13] = "AUDJPY";
      trade_tools[14] = "GBPCHF";
      trade_tools[15] = "CHFJPY";
      trade_tools[16] = "NZDJPY";
      trade_tools[17] = "AUDCAD";
      trade_tools[18] = "AUDNZD";
      trade_tools[19] = "CADJPY";
      trade_tools[20] = "USOIL";
   }

   if (StringFind(company, "Henyep Investment") == 0)
   {
      trade_tools[0] = "SPT_GLD";
      trade_tools[1] = "SPT_SVR";
      trade_tools[2] = "EURUSD";
      trade_tools[3] = "USDJPY";
      trade_tools[4] = "GBPUSD";
      trade_tools[5] = "USDCHF";
      trade_tools[6] = "AUDUSD";
      trade_tools[7] = "EURJPY";
      trade_tools[8] = "EURGBP";
      trade_tools[9] = "EURCHF";
      trade_tools[10] = "GBPJPY";
      trade_tools[11] = "USDCAD";
      trade_tools[12] = "";
      trade_tools[13] = "AUDJPY";
      trade_tools[14] = "GBPCHF";
      trade_tools[15] = "";
      trade_tools[21] = "GBPCAD";
   }

   if (company == "Global Market Index Corporation")
   {
      trade_tools[0] = "XAUUSD'";
      trade_tools[1] = "XAGUSD'";
      trade_tools[2] = "EURUSD'";
      trade_tools[3] = "USDJPY'";
      trade_tools[4] = "GBPUSD'";
      trade_tools[5] = "USDCHF'";
      trade_tools[6] = "AUDUSD'";
      trade_tools[7] = "EURJPY'";
      trade_tools[8] = "EURGBP'";
      trade_tools[9] = "EURCHF'";
      trade_tools[11] = "USDCAD'";
      trade_tools[12] = "NZDUSD'";
   }

   if (StringFind(company, "Australia AFT Finance") == 0)
   {
      trade_tools[0] = "SPT_GLD";
      trade_tools[1] = "";
      trade_tools[2] = "EURUSD.";
      trade_tools[3] = "USDJPY.";
      trade_tools[4] = "GBPUSD.";
      trade_tools[5] = "USDCHF.";
      trade_tools[6] = "AUDUSD.";
      trade_tools[7] = "EURJPY.";
      trade_tools[8] = "EURGBP.";
      trade_tools[9] = "EURCHF.";
      trade_tools[11] = "USDCAD.";
      trade_tools[12] = "";
      trade_tools[13] = "AUDJPY.";
      trade_tools[14] = "GBPCHF.";
      trade_tools[15] = "CHFJPY.";
      trade_tools[16] = "";
      trade_tools[17] = "AUDCAD.";
      trade_tools[18] = "";
      trade_tools[19] = "CADJPY.";
      trade_tools[20] = "";
      trade_tools[21] = "GBPCAD.";
      trade_tools[22] = "AUDCHF.";
   }

   if (company == "X-Trade Brokers")
   {
      trade_tools[0] = "GOLDs";
      trade_tools[1] = "SILVERs";
      trade_tools[2] = "EURUSD";
      trade_tools[3] = "USDJPY";
      trade_tools[4] = "GBPUSD";
      trade_tools[5] = "USDCHF";
      trade_tools[6] = "AUDUSD";
      trade_tools[7] = "EURJPY";
      trade_tools[8] = "";
      trade_tools[9] = "";
      trade_tools[10] = "GBPJPY";
      trade_tools[11] = "USDCAD";
      trade_tools[12] = "NZDUSD";
      trade_tools[13] = "";
      trade_tools[14] = "";
      trade_tools[15] = "";
      trade_tools[16] = "";
      trade_tools[17] = "";
      trade_tools[18] = "";
      trade_tools[19] = "";
      trade_tools[20] = "OILs";
   }

   if (StringFind(company, "KVB Kunlun") == 0)
   {
      trade_tools[0] = "XAUUSD";
      trade_tools[2] = "EURUSD";
      trade_tools[3] = "USDJPY";
      trade_tools[4] = "GBPUSD";
      trade_tools[5] = "USDCHF";
      trade_tools[6] = "AUDUSD";
      trade_tools[7] = "EURJPY";
      trade_tools[8] = "EURGBP";
      trade_tools[9] = "EURCHF";
      trade_tools[10] = "GBPJPY";
      trade_tools[11] = "USDCAD";
      trade_tools[12] = "NZDUSD";
      trade_tools[13] = "AUDJPY";
      trade_tools[14] = "GBPCHF";
      trade_tools[15] = "CHFJPY";
      trade_tools[16] = "NZDJPY";
      trade_tools[17] = "AUDCAD";
      trade_tools[18] = "AUDNZD";
      trade_tools[19] = "CADJPY";
      trade_tools[20] = "USOIL";
      trade_tools[23] = "AUDCNY";
      trade_tools[24] = "NZDCNY";
   }
}

int symbol_to_index(string symbol)
{
   if (symbol == "XAUUSD")
      return(0);
   if (symbol == "XAGUSD")
      return(1);
   if (symbol == "EURUSD")
      return(2);
   if (symbol == "USDJPY")
      return(3);
   if (symbol == "GBPUSD")
      return(4);
   if (symbol == "USDCHF")
      return(5);
   if (symbol == "AUDUSD")
      return(6);
   if (symbol == "EURJPY")
      return(7);
   if (symbol == "EURGBP")
      return(8);
   if (symbol == "EURCHF")
      return(9);
   if (symbol == "GBPJPY")
      return(10);
   if (symbol == "USDCAD")
      return(11);
   if (symbol == "NZDUSD")
      return(12);
   if (symbol == "AUDJPY")
      return(13);
   if (symbol == "GBPCHF")
      return(14);
   if (symbol == "CHFJPY")
      return(15);
   if (symbol == "NZDJPY")
      return(16);
   if (symbol == "AUDCAD")
      return(17);
   if (symbol == "AUDNZD")
      return(18);
   if (symbol == "CADJPY")
      return(19);
   if (symbol == "USOIL")
      return(20);
   if (symbol == "GBPCAD")
      return(21);
   if (symbol == "AUDCHF")
      return(22);
   if (symbol == "AUDCNY")
      return(23);
   if (symbol == "NZDCNY")
      return(24);
}

string trade_symbol(string symbol)
{
   int index = symbol_to_index(symbol);
   return (trade_tools[index]);
}

int login()
{
   int error;
   error = ConnectToHost("soft.em7788.com");
   retry_login_count = retry_login_count + 1;
   if(error > 0)
      socket = error;
   else
   {
      Print("�޷����ӵ�����������������״̬");
      CloseConnection(socket);
      return(-1);
   }

   SetTCPNoDelay(socket, 1);

   error = SendAuthMessage(socket, username, password, TerminalCompany(), AccountNumber());
   if(error != 0)
   {
      Print("��֤���������û��������롣");
      CloseConnection(socket);
      return(-1);
   }

   if(SetRecvTimeOut(socket, 1000) != 0)
   {
      Print("�������ô���");
      CloseConnection(socket);
      return(-1);
   }

   Print("��½�ɹ�");

   retry_login_count = 0;
   return(0);
}

void show_text()
{
   if(ObjectCreate("EAClient_text1",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text1", "Ͷ��Я����", 12, "����ϸ��", Violet);
      ObjectSet("EAClient_text1", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text1", OBJPROP_YDISTANCE, 40);
   }
   if(ObjectCreate("EAClient_text2",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text2", "����ϵͳ", 12, "����ϸ��", Violet);
      ObjectSet("EAClient_text2", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text2", OBJPROP_YDISTANCE, 56);
   }
   if(ObjectCreate("EAClient_text3",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text3", "�������ԲƸ�����������", 12, "����ϸ��", Violet);
      ObjectSet("EAClient_text3", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text3", OBJPROP_YDISTANCE, 72);
   }
   if(ObjectCreate("EAClient_text4",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text4", "Alex Zhao", 12, "����ϸ��", Violet);
      ObjectSet("EAClient_text4", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text4", OBJPROP_YDISTANCE, 88);
   }
}

void dynamic_text()
{
   if(ObjectCreate("EAClient_username", OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_username", "������¼�û���: "+username, 10, "����ϸ��", Violet);
      ObjectSet("EAClient_username", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_username", OBJPROP_YDISTANCE, 104);
   }

   if(ObjectCreate("EAClient_password", OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_password", "��¼����: "+password, 10, "����ϸ��", Violet);
      ObjectSet("EAClient_password", OBJPROP_XDISTANCE, 240);
      ObjectSet("EAClient_password", OBJPROP_YDISTANCE, 104);
   }

   if(ObjectCreate("EAClient_enable", OBJ_LABEL, 0, 0, 0))
   {
      if(�Զ�����)
         ObjectSetText("EAClient_enable", "�����Զ�����", 10, "����ϸ��", Red);
      else
         ObjectSetText("EAClient_enable", "��ֹ�Զ�����", 10, "����ϸ��", Red);
      ObjectSet("EAClient_enable", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_enable", OBJPROP_YDISTANCE, 118);
   }

   if(ObjectCreate("EAClient_type", OBJ_LABEL, 0, 0, 0))
   {
      if(���ֱֲ�������)
         ObjectSetText("EAClient_type", "���ֱֲ�������", 10, "����ϸ��", Red);
      else
         ObjectSetText("EAClient_type", "����������", 10, "����ϸ��", Red);
      ObjectSet("EAClient_type", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_type", OBJPROP_YDISTANCE, 132);
   }

   if(ObjectCreate("EAClient_alive",OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_alive", "�ϴ�ͬ��ʱ�� " + TimeToStr(TimeCurrent(),TIME_DATE|TIME_SECONDS), 10, "����ϸ��", Violet);
      ObjectSet("EAClient_alive", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_alive", OBJPROP_YDISTANCE, 146);
   }
}

void update_text()
{
   ObjectSetText("EAClient_alive", "�ϴ�ͬ��ʱ�� " + TimeToStr(TimeCurrent(),TIME_DATE|TIME_SECONDS), 10, "����ϸ��", Violet);
}

int init()
{
    retry_login_count = 0;
    initialized = false;
    retry_warn = true;
    setting_error = false;
    Print("ȫ�����ϵͳ�ͻ��˳�ʼ��   ",TerminalCompany());
    init_table();
    if(�û��� != "" && ��½���� != "")
    {
      username = �û���;
      password = ��½����;
    }

    if(IsDllsAllowed() == false)
    {
       MessageBox("DLL ���ò��������ܽ��ײ�������", "���ô���", MB_YESNO|MB_ICONQUESTION);
       setting_error = true;
       return(0);
    }

    if(�Զ����� && IsTradeAllowed() == false)
    {
        MessageBox("��������Զ�����", "���ô���", MB_YESNO|MB_ICONQUESTION);
        setting_error = true;
        return(0);
    }

    InitNetwork();

    if(login()<0)
    {
        Print("��½����������");
        return(0);
    }

    show_text();

    datetime current = TimeCurrent();
    string filename = TimeToStr(current,TIME_DATE);
    filehandle = FileOpen(filename, FILE_READ | FILE_WRITE);
    if(filehandle < 0)
    {
        filehandle = FileOpen(filename, FILE_WRITE);
    }

    FileSeek(filehandle, 0, SEEK_END);

    initialized = true;

    Print("����ϵͳ��ʼ���ɹ�");

    LotSize = MarketInfo(Symbol(), MODE_LOTSIZE);
    MinLot = MarketInfo(Symbol(), MODE_MINLOT);
    LotStep = MarketInfo(Symbol(), MODE_LOTSTEP);
    MaxLot = MarketInfo(Symbol(), MODE_MAXLOT);

    prev_last_order = OrdersTotal();
    prev_close_order = OrdersHistoryTotal();
    dynamic_text();
    return (0);
}

int deinit()
{
//----
    FileClose(filehandle);
    CloseConnection(socket);
    DeInitNetwork();
//----
    return(0);
}

bool double_equal(double s1, double s2)
{
   if(MathAbs(s1-s2) < 0.00000001)
      return(true);
   else
      return(false);
}

double price_per_vol(string symbol, double price)
{
   if(StringFind(symbol, "XAU", 0) != -1)
      return(price);

   if(StringFind(symbol, "XAG", 0) != -1)
      return(1000);

   if(StringFind(symbol, "OIL", 0) != -1)
      return(price*10);

   return(1000);
}

int MakeOrder(string symbol, int cmd, double percent, double volume, double price, int slippoint, double stoploss, double takeprofit,int ticket,int openclose, int opid)
{
   double lots;
   double current_price = 0;
   double max_slip = 0;
   int i;
   int OrderTotal;
   RefreshRates();
   price = MathRound(price*10000)/10000;
   if(���ֱֲ�������)
   {
      lots = (AccountBalance() * percent / price_per_vol(symbol, price) )*10;
      lots = MathRound(lots)/10;
   }
   else
   {
      lots = volume;
   }
   if(lots < MinLot)
      lots = MinLot;
   if(lots > MaxLot)
      lots = MaxLot;
   if( openclose == 1 )
   {
      int tick;

      if (cmd == OP_SELL)
         current_price = MarketInfo(trade_symbol(symbol), MODE_BID);
      if (cmd == OP_BUY)
         current_price = MarketInfo(trade_symbol(symbol), MODE_ASK);

      if ( MathAbs(price - current_price) < trade_slip[symbol_to_index(symbol)] )
         price = current_price;

      tick = OrderSend(trade_symbol(symbol), cmd, lots, price, 0, stoploss, takeprofit, "�Զ�����", ticket, 0, Red);
      if(tick < 0)
      {
	      Print("��������", GetLastError(),"����: ", lots, "  ���Ҷ�: ",symbol, "�۸�: ",price );
	   }
	   else
	   {
	      Print("���͸�����������");
	      SendOrderAck(socket, trade_symbol(symbol), cmd, slippoint, lots, percent,
                      price, stoploss, takeprofit, 0, 1/*open*/,tick, opid);
	   }
   }
   else if( openclose == 0 )
   {
      OrderTotal = OrdersTotal();
      for(i=0;i<OrderTotal;i++)
      {
         if(OrderSelect(i,SELECT_BY_POS)==true)
         {
            if(OrderMagicNumber() == ticket)
            {
               if(!OrderClose(OrderTicket(), OrderLots(), price, slippoint))
               {
                  if( GetLastError() == ERR_INVALID_TICKET )
                  {
                     Alert("ƽ����ϢΪ����ȡ��");
                     OrderDelete( OrderTicket() );
                  }
                  else
                  {
                     Print("ƽ�ִ���", GetLastError());
                     if(OrderType() == OP_BUY)
                        OrderClose(OrderTicket(), OrderLots(), MarketInfo(OrderSymbol(), MODE_BID), 0);
                     else
                        OrderClose(OrderTicket(), OrderLots(), MarketInfo(OrderSymbol(), MODE_ASK), 0);
                  }
               }
               Print("���͸���ƽ������");
      	      SendOrderAck(socket, OrderSymbol(), OrderType(), 0, OrderLots(), percent,
                            OrderClosePrice(), OrderStopLoss(), OrderTakeProfit(), OrderProfit(), 0/*close*/,OrderTicket(), opid);
            }
         }
      }
   }
   else if( openclose == 2)
   {
      OrderTotal = OrdersTotal();
      for(i=0;i<OrderTotal;i++)
      {
         if(OrderSelect(i,SELECT_BY_POS) == true)
         {
            if(OrderMagicNumber() == ticket)
            {
               OrderModify(OrderTicket(),price,stoploss,takeprofit,0,Blue);
               Print("�����޸Ĵ���",GetLastError(), "��λ:",price, "ֹ��:",stoploss,"  ֹӯ:",takeprofit);
            }
         }
      }
   }

   return(0);
}
//+------------------------------------------------------------------+
//| expert start function                                            |
//+------------------------------------------------------------------+

int start()
{
    if(setting_error)
       return(0);

    while (initialized)
    {
        int cmd[1];
        int slippage[1];
        double volume[1];
        double percent[1];
        double price[1];
        double stoploss[1];
        double takeprofit[1];
        int operate[1];
        int ticket[1];
        int opid[1];
        string readbuf = "                ";

        int status;
        status = ReceiveFromHost(socket, readbuf, cmd, slippage, volume, percent, price, stoploss, takeprofit, operate, ticket, opid );
        if (status == 0)
        {
            string symbol = StringTrimRight(readbuf);

            string opcmd;
            if(cmd[0] == OP_SELL && operate[0] == 1)
                opcmd = "��������";
            else if(cmd[0] == OP_BUY && operate[0] == 1)
                opcmd = "������";
            else if(cmd[0] == OP_SELL && operate[0] == 0)
                opcmd = "ƽ������";
            else if(cmd[0] == OP_BUY && operate[0] == 0)
                opcmd = "ƽ����";
            else if(operate[0] == 2)
                opcmd = "�޸����ж���";

            if(cmd[0] == OP_BUYLIMIT && operate[0] == 1)
               opcmd = "�����޼�����";
            else if(cmd[0] == OP_BUYLIMIT && operate[0] == 0)
               opcmd = "ƽ���޼�����";
            else if(cmd[0] == OP_BUYSTOP && operate[0] == 1)
               opcmd = "����ͻ������";
            else if(cmd[0] == OP_BUYSTOP && operate[0] == 0)
               opcmd = "ƽ��ͻ������";

            if(cmd[0] == OP_SELLLIMIT && operate[0] == 1)
               opcmd = "�����޼�����";
            else if(cmd[0] == OP_SELLLIMIT && operate[0] == 0)
               opcmd = "ƽ���޼�����";
            else if(cmd[0] == OP_SELLSTOP && operate[0] == 1)
               opcmd = "����ͻ������";
            else if(cmd[0] == OP_BUYSTOP && operate[0] == 0)
               opcmd = "ƽ��ͻ������";

            string objname;
            objname = DoubleToStr(ticket[0], 0);
            string message;

            if(cmd[0] == OP_SELL || cmd[0] == OP_BUY)
            {
               if(operate[0] == 1)
               {
                  message = "�м۶�������:  "+symbol+"  �ɽ��۸�:  "+DoubleToStr(price[0],4)+"  ������λ:  "+DoubleToStr(percent[0]*100,4)+"%"+
                            "  ��������:  "+DoubleToStr(volume[0],2)+"  ����ֹ��:  "+DoubleToStr(stoploss[0],4)+"  ��������:  "+DoubleToStr(takeprofit[0],4);
               }
               else
               {
                  message = "�м۶�������:  "+symbol;
               }
               Alert( "�µĶ���", opcmd, ":   ", message);
//                Alert("��ע�����¶�����  ",opcmd, "    ����Ʒ��:  ", symbol, "  �ɽ��۸�:   ", price[0], "   ������λ:  ", volume[0]*100,"%");
               if (�Զ�����)
                  MakeOrder(symbol,cmd[0],percent[0],volume[0],price[0],slippage[0],stoploss[0],takeprofit[0],ticket[0],operate[0],opid[0]);
               FileWrite(filehandle, "��ʷ������  ", opcmd, "����Ʒ��:  ", symbol, "�ɽ��۸� ", price[0],"��λ��  ", volume[0]*100,"%" );
               FileFlush(filehandle);
            }

            if(cmd[0] == OP_BUYLIMIT || cmd[0] == OP_BUYSTOP)
            {
               if(operate[0] == 1)
               {
                  message = "�ҵ����뽻��:  "+symbol+"  �ɽ��۸�:  "+DoubleToStr(price[0],4)+"  ������λ:  "+DoubleToStr(percent[0]*100,4)+"%"+
                            "  ��������:  "+DoubleToStr(volume[0],2)+"  ����ֹ��:  "+DoubleToStr(stoploss[0],4)+"  ��������:  "+DoubleToStr(takeprofit[0],4);
               }
               else
               {
                  message = "�ҵ����뽻��:   "+symbol;
               }
               Alert("�µĶ���", opcmd, ":   ", message);
//                MessageBox(message,"�µĶ���"+opcmd);
//                Alert("�ҵ����뽻��:   ",opcmd,"   ����Ʒ��:", symbol, "   �ɽ��۸�  ", price[0], "   ֹ���λ:  ", stoploss[0],
//                      "   ������λ��  ", takeprofit[0], "   ��λ��  ", volume[0]*100,"%");
               if (�Զ�����)
                  MakeOrder(symbol,cmd[0],percent[0],volume[0],price[0],slippage[0],stoploss[0],takeprofit[0],ticket[0],operate[0],opid[0]);
               FileWrite(filehandle, "��ʷ������  ", opcmd, "����Ʒ��:  ", symbol, "�ɽ��۸� ",price[0], "ֹ���λ:  ",stoploss[0],"   ������λ��",takeprofit[0],"��λ��  ", volume[0]*100,"%" );
               FileFlush(filehandle);
            }

            if(cmd[0] == OP_SELLLIMIT || cmd[0] == OP_SELLSTOP)
            {
               if(operate[0] == 1)
               {
                  message = "�ҵ���������:  "+symbol+"  �ɽ��۸�:  "+DoubleToStr(price[0],4)+"  ������λ:  "+DoubleToStr(percent[0]*100,4)+"%"+
                            "  ��������:  "+DoubleToStr(volume[0],2)+"  ����ֹ��:  "+DoubleToStr(stoploss[0],4)+"  ��������:  "+DoubleToStr(takeprofit[0],4);
               }
               else
               {
                  message = "�ҵ���������:   "+symbol;
               }
               Alert("�µĶ���",opcmd,":   ",message);
//                MessageBox(message,"�µĶ���"+opcmd);
//                Alert("�ҵ����뽻��:   ",opcmd,"   ����Ʒ��:", symbol, "   �ɽ��۸�  ", price[0], "   ֹ���λ:  ", stoploss[0],
//                      "   ������λ��  ", takeprofit[0], "   ��λ��  ", volume[0]*100,"%");
               if (�Զ�����)
                  MakeOrder(symbol,cmd[0],percent[0],volume[0],price[0],slippage[0],stoploss[0],takeprofit[0],ticket[0],operate[0],opid[0]);
               FileWrite(filehandle, "��ʷ������  ", opcmd, "����Ʒ��:  ", symbol, "�ɽ��۸� ",price[0], "ֹ���λ:  ",stoploss[0],"   ������λ��",takeprofit[0],"��λ��  ", volume[0]*100,"%" );
               FileFlush(filehandle);
            }

            if(symbol == Symbol())
            {
                if(cmd[0] == OP_BUY)
                {
                    ObjectCreate(objname, OBJ_ARROW, 0, Time[0], 0);
                    ObjectSet(objname, OBJPROP_ARROWCODE, SYMBOL_ARROWUP);
                    ObjectSet(objname, OBJPROP_PRICE1, price[0]);
                }
                else if(cmd[0] == OP_SELL)
                {
                    ObjectCreate(objname, OBJ_ARROW, 0, Time[0], 0);
                    ObjectSet(objname, OBJPROP_ARROWCODE, SYMBOL_ARROWDOWN);
                    ObjectSet(objname, OBJPROP_PRICE1, price[0]);
                }
            }

            /*ƽ�ֻ�����ʾ����*/
            int close_total = OrdersHistoryTotal();
            if(close_total>prev_close_order)
            {
               int close_item;
               for(close_item = close_total - 1;close_item>=prev_close_order;close_item--)
               {
                  if(OrderSelect(close_item,SELECT_BY_POS,MODE_HISTORY)==true)
                  {
                     if(OrderProfit() > 0)
                        PlaySound("����ƽ��.wav");
                     else
                        PlaySound("����ƽ��.wav");
                  }
               }

               prev_close_order = close_total;
            }
            update_text();
        }
        else if(status < 0)
        {
            initialized = false;
        }
        else
        {
            update_text();
        }
    }

    if(retry_login_count >=3 && retry_warn)
    {
       int ret;
       ret = MessageBox("��ƽ̨ϵͳ��¼�������˳����ԣ�", "��¼ʧ��", MB_YESNO|MB_ICONQUESTION);
       if (ret == IDNO)
          retry_warn = false;
       return(0);
    }

    if(retry_login_count < 3)
    {
       if(login()==0)
          initialized = true;
       else
          initialized = false;
    }

    return(0);
}
//+-------------------------------------------
