//+------------------------------------------------------------------+
//|                                       快速跟单系统 操作员端         |
//|                                                                  |
//|                                                                  |
//+------------------------------------------------------------------+
#property copyright "Zhao Zhe"
#property link      "Alex Zhao"
#property show_inputs

#import "EAClient.dll"
int InitNetwork();
int DeInitNetwork();
int ConnectToHost(string HostName);
int CloseConnection(int ClientSock);
int SendAuthMessage(int ClientSock, string user, string key, string MM, int MT4Account);
int ReceiveFromHost(int ClientSock, string symbol, int &cmd[], int &slippage[], double &volume[], double &percent[],
                    double &price[], double &stoploss[], double &takeprofit[], int &operate[], int &ticket[]);
int SendOrder(int ClientSock, string symbol, int cmd, int slippage,
              double volume, double percent, double price, double stoploss, double takeprofit, double profit,
              int operate, int ticket);
int SetRecvTimeOut(int ClientSock, int TimeOut);
int SetTCPNonBlock(int ClientSock);
int SetTCPNoDelay(int ClientSock, int enable);
int ConnectionStatusDetect(int ClientSock);
#import

#include <WinUser32.mqh>
#include <stderror.mqh>

extern string 用户名 = "";
extern string 登陆密码 = "";
extern int 跟单滑点 = 5;


bool initialized = false;
bool retry_warn = true;
bool setting_error = true;
int socket;

string username = "demo";
string password = "demo";

int prev_last_order;
int prev_close_order;
int retry_login_count = 0;
int slippoint = 0;
double percent;
//+------------------------------------------------------------------+
//| expert initialization function                                   |
//+------------------------------------------------------------------+


double open_order_openprice[100];
double open_order_takeprofit[100];
double open_order_stoploss[100];
int open_order_ticket[100];
int open_order_count;

int login()
{
    int error;

    error = ConnectToHost("soft.em7788.com");
    retry_login_count = retry_login_count + 1;
    if(error > 0)
       socket = error;
    else
    {
       Print("无法连接到服务器，请检查网络状态");
       return(-1);
    }

    SetTCPNoDelay(socket, 1);

    error = SendAuthMessage(socket, username, password, TerminalCompany(), AccountNumber());
    if(error != 0)
    {
       Print("认证错误，请检查用户名，密码。");
       CloseConnection(socket);
       return(-1);
    }

    if(SetTCPNonBlock(socket) != 0)
    {
       Print("连接设置错误");
       CloseConnection(socket);
       return(-1);
    }

    Print("登陆成功");

    retry_login_count = 0;
    slippoint = 跟单滑点;
    return(0);
}

void show_text()
{
   if(ObjectCreate("EAClient_text1",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text1", "投资携万利", 12, "华文细黑", Violet);
      ObjectSet("EAClient_text1", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text1", OBJPROP_YDISTANCE, 40);
   }
   if(ObjectCreate("EAClient_text2",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text2", "交易员喊单系统", 12, "华文细黑", Violet);
      ObjectSet("EAClient_text2", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text2", OBJPROP_YDISTANCE, 56);
   }
   if(ObjectCreate("EAClient_text3",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text3", "操盘手必备利器", 12, "华文细黑", Violet);
      ObjectSet("EAClient_text3", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text3", OBJPROP_YDISTANCE, 72);
   }
   if(ObjectCreate("EAClient_text4",OBJ_LABEL, 0, 0, 0 ))
   {
      ObjectSetText("EAClient_text4", "Zhao Zhe", 12, "华文细黑", Violet);
      ObjectSet("EAClient_text4", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_text4", OBJPROP_YDISTANCE, 88);
   }
}

void dynamic_text()
{
   if(ObjectCreate("EAClient_username", OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_username", "跟单登录用户名: "+username, 10, "华文细黑", Violet);
      ObjectSet("EAClient_username", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_username", OBJPROP_YDISTANCE, 104);
   }

   if(ObjectCreate("EAClient_password", OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_password", "登录密码: "+password, 10, "华文细黑", Violet);
      ObjectSet("EAClient_password", OBJPROP_XDISTANCE, 240);
      ObjectSet("EAClient_password", OBJPROP_YDISTANCE, 104);
   }

   if(ObjectCreate("EAClient_alive",OBJ_LABEL, 0, 0, 0))
   {
      ObjectSetText("EAClient_alive", "上次同步时间 " + TimeToStr(TimeCurrent(),TIME_DATE|TIME_SECONDS), 10, "华文细黑", Violet);
      ObjectSet("EAClient_alive", OBJPROP_XDISTANCE, 10);
      ObjectSet("EAClient_alive", OBJPROP_YDISTANCE, 118);
   }
}

void update_text()
{
   ObjectSetText("EAClient_alive", "上次同步时间 " + TimeToStr(TimeCurrent(),TIME_DATE|TIME_SECONDS), 10, "华文细黑", Violet);
}

bool not_equal(double s1, double s2)
{
   if(MathAbs(s1-s2) > 0.0000001)
      return(true);
   else
      return(false);
}

int init()
{
//----
    initialized = false;
    retry_warn = true;
    setting_error = false;
    retry_login_count = 0;
    if(用户名 != "" && 登陆密码 != "")
    {
      username = 用户名;
      password = 登陆密码;
    }
    if(IsDllsAllowed() == false)
    {
        MessageBox("DLL 调用不允许，智能交易不能运行", "设置错误", MB_YESNO|MB_ICONQUESTION);
        setting_error = true;
        return(0);
    }

    InitNetwork();
    if(login()==0)
        initialized = true;
    else
    {
        initialized = false;
        return(0);
    }

    show_text();
    prev_last_order = OrdersTotal();
    prev_close_order = OrdersHistoryTotal();
    open_order_count = OrdersTotal();

    int i;
    for(i=0;i<100;i++)
    {
       open_order_openprice[i] = 0;
       open_order_takeprofit[i] = 0;
       open_order_stoploss[i] = 0;
       open_order_ticket[i] = 0;
    }

    for(i=0;i<open_order_count;i++)
    {
       if(OrderSelect(i,SELECT_BY_POS) == true)
       {
          open_order_openprice[i] = OrderOpenPrice();
          open_order_takeprofit[i] = OrderTakeProfit();
          open_order_stoploss[i] = OrderStopLoss();
          open_order_ticket[i] = OrderTicket();
       }
    }
    dynamic_text();
//----
    return(0);
}
//+------------------------------------------------------------------+
//| expert deinitialization function                                 |
//+------------------------------------------------------------------+
int deinit()
{
//----
    CloseConnection(socket);
    DeInitNetwork();
//----
    return(0);
}
//+------------------------------------------------------------------+
//| expert start function                                            |
//+------------------------------------------------------------------+
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

   if(StringFind(symbol, "GLD", 0) != -1)
      return(price);

   if(StringFind(symbol, "SVR", 0) != -1)
      return(1000);

   if(StringFind(symbol, "XAG", 0) != -1)
      return(1000);

   if(StringFind(symbol, "OIL", 0) != -1)
      return(price*10);

   return(1000);
}

string short_symbol(string symbol)
{
   string st = "";
   int c;
   int i,j = 0;

   if(StringFind(symbol, "GLD", 0) != -1)
      return("XAUUSD");

   if(StringFind(symbol, "SVR", 0) != -1)
      return("XAGUSD");

   for(i=0;i<StringLen(symbol);i++)
   {
      c = StringGetChar(symbol,i);
      if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
      {
         st = StringSetChar(st,j,c);
         j = j + 1;
      }
   }
   return(st);
}

int start()
{
    double lots;
    int total;
    int close_total;
    int open_item;
    int close_item;
    int item;

    if(setting_error)
       return(0);

    while(initialized)
    {
        if(ConnectionStatusDetect(socket) != 0)
        {
           initialized = false;
           break;
        }

        update_text();

        total = OrdersTotal();
        if(total > prev_last_order)
        {
            for( open_item = total - 1; open_item >= prev_last_order; open_item --)
            {
                if(OrderSelect(open_item, SELECT_BY_POS) == true)
                {
                    percent = price_per_vol(OrderSymbol(), OrderOpenPrice()) * OrderLots() / AccountBalance();
                    if (SendOrder( socket, short_symbol(OrderSymbol()), OrderType(), slippoint, OrderLots(), percent,
                               OrderOpenPrice(), OrderStopLoss(), OrderTakeProfit(), OrderProfit(), 1/*open*/, OrderTicket() ) == 0)
                        Print("开仓订单发出  ",OrderSymbol(),"  订单手数  ",OrderLots(),"  订单仓位  ",percent,"  订单开盘价  ",OrderOpenPrice(),
                              "  订单止损  ",OrderStopLoss(),"  订单获利  ",OrderTakeProfit());
                    else
                        initialized = false;
                }
            }
            prev_last_order = total;

            open_order_count = OrdersTotal();
            for(open_item = 0; open_item < open_order_count; open_item ++)
            {
               if(OrderSelect(open_item,SELECT_BY_POS) == true)
               {
                  open_order_openprice[open_item] = OrderOpenPrice();
                  open_order_takeprofit[open_item] = OrderTakeProfit();
                  open_order_stoploss[open_item] = OrderStopLoss();
                  open_order_ticket[open_item] = OrderTicket();
               }
            }
        }

        close_total = OrdersHistoryTotal();
        if(close_total > prev_close_order)
        {
            for(close_item = close_total -1; close_item >= prev_close_order; close_item --)
            {
                if(OrderSelect(close_item, SELECT_BY_POS, MODE_HISTORY)==true)
                {

                    if (SendOrder( socket, short_symbol(OrderSymbol()), OrderType(), slippoint, 0, 0,
                                   OrderClosePrice(), OrderStopLoss(), OrderTakeProfit(), OrderProfit(), 0/*close*/, OrderTicket() ) == 0)
                        Print("平仓订单发出  ",OrderSymbol(),"  订单开盘价  ",OrderOpenPrice(),
                              "  订单止损  ",OrderStopLoss(),"  订单获利  ",OrderTakeProfit());
                    else
                        initialized = false;
                }
            }
            prev_close_order = close_total;
            prev_last_order = OrdersTotal();

            open_order_count = OrdersTotal();
            for(item = 0; item < open_order_count; item ++)
            {
               if(OrderSelect(open_item,SELECT_BY_POS) == true)
               {
                  open_order_openprice[item] = OrderOpenPrice();
                  open_order_takeprofit[item] = OrderTakeProfit();
                  open_order_stoploss[item] = OrderStopLoss();
                  open_order_ticket[item] = OrderTicket();
               }
            }
        }

        if(total == prev_last_order)
        {
           for(item = 0; item<open_order_count;item++)
           {
              if(OrderSelect(item,SELECT_BY_POS) == true)
              {
                 if(open_order_ticket[item] == OrderTicket())
                 {
                    if( not_equal(OrderOpenPrice(),open_order_openprice[item]) ||
                        not_equal(OrderStopLoss(),open_order_stoploss[item]) ||
                        not_equal(OrderTakeProfit(),open_order_takeprofit[item]) )
                    {
                       if (SendOrder( socket, short_symbol(OrderSymbol()), OrderType(), slippoint, 0, 0,
                                      OrderOpenPrice(), OrderStopLoss(), OrderTakeProfit(), OrderProfit(), 2/*modify*/, OrderTicket() ) == 0)
                          Print("修改订单发出  ",OrderSymbol(),"  订单开盘价  ",OrderOpenPrice(),
                                "  订单止损  ",OrderStopLoss(),"  订单获利  ",OrderTakeProfit());
                       else
                          initialized = false;

                       open_order_openprice[item] = OrderOpenPrice();
                       open_order_stoploss[item] = OrderStopLoss();
                       open_order_takeprofit[item] = OrderTakeProfit();
                    }
                 }
              }
           }
        }
        Sleep(100);
    }

    if(retry_login_count >=3 && retry_warn)
    {
       int ret;
       ret = MessageBox("跨平台系统登录错误，请退出重试！", "登录失败", MB_YESNO|MB_ICONQUESTION);
       if(ret == IDNO)
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

//+----------------------------------------------------------------
