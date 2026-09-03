/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收碳排因子信息
质量数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_91t801_rcv)
int f_tk00_setco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_cm_91t801_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString raw_data = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CString SeqNo = "";
	CModel ttk0004c("TTK0004C");

	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Rows[i]["ID"].ToString().Trim() == "")
			{
				ttk0004c["ID"] = 0;
			}
			else
			{
				ttk0004c["ID"] = bcls_rec->Tables[0].Rows[i]["ID"].ToString();
			}
			//raw_data = bcls_rec->Tables[0].Rows[i]["RAW_DATA"].ToString();
			//ttk0004c["MAT_CODE"] = raw_data.SubstringNE(raw_data.Find("_"), raw_data.GetLength() - raw_data.Find("_"));
			//ttk0004c["MAT_NAME"] = raw_data.SubstringNE(0, raw_data.Find("_"));
			ttk0004c["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["RAW_DATA_CODE"].ToString().SubstringNE(0, 20);
			ttk0004c["MAT_NAME"] = bcls_rec->Tables[0].Rows[i]["RAW_DATA_NAME"].ToString().SubstringNE(0, 100);
			ttk0004c["DATA_FROM"] = bcls_rec->Tables[0].Rows[i]["BACKGROUND_DATA"].ToString().SubstringNE(0, 100);
			ttk0004c["TYPE_DESC"] = bcls_rec->Tables[0].Rows[i]["FACTOR"].ToString().SubstringNE(0, 100);
			ttk0004c["CO2_COE"] = bcls_rec->Tables[0].Rows[i]["VALUE"].ToDecimal().Round(10);
			ttk0004c["VALID_TIME"] = bcls_rec->Tables[0].Rows[i]["VERSION"].ToString();
			Log::Info("", __FUNCTION__, "22222");
			ttk0004c["VALID_TIME"] = ttk0004c["VALID_TIME"].ToString().Replace("-", "");
			Log::Info("", __FUNCTION__, "333333333333");
			ttk0004c["TYPE"] = bcls_rec->Tables[0].Rows[i]["TYPE"].ToString().SubstringNE(0,20);
			ttk0004c["UNIT"] = bcls_rec->Tables[0].Rows[i]["UNIT"].ToString();
			Log::Info("", __FUNCTION__, "4444444444");
			if (ttk0004c["MAT_CODE"].ToString() == "70202")
			{
				ttk0004c["MAT_CODE"] = "12300";
			}
			if (ttk0004c["MAT_CODE"].ToString() == "70201" || ttk0004c["MAT_CODE"].ToString() == "36200")
			{
				ttk0004c["MAT_CODE"] = "12301";
			}

			ttk0004c["REC_CREATE_TIME"] = datetime;
			ttk0004c["REC_CREATOR"] = s.userid;
			ttk0004c.TrimOrBlank();
			//焦炉煤气,转炉煤气,蒸汽，天然气使用GJ
			if (ttk0004c["MAT_CODE"].ToString() == "48081" || ttk0004c["MAT_CODE"].ToString() == "49092" || ttk0004c["MAT_CODE"].ToString() == "49094" || ttk0004c["MAT_CODE"].ToString() == "49280" || ttk0004c["MAT_CODE"].ToString() == "58001" || ttk0004c["MAT_CODE"].ToString() == "59200" || ttk0004c["MAT_CODE"].ToString() == "49093")
			{
				if (ttk0004c["UNIT"].ToString() == "kgCO2/MJ" || ttk0004c["DATA_FROM"].ToString().Find("按热值计",0) >1)
				{
					ttk0004c.Delete("MAT_CODE,TYPE_DESC,VALID_TIME,TYPE");
					ttk0004c.Insert();
				}
			}
			else
			{
				ttk0004c.Delete("MAT_CODE,TYPE_DESC,VALID_TIME,TYPE");
				ttk0004c.Insert();
			}
			
		}

		//将数据插入到碳排因子中去
		EIClass inBlock_yz, outBlock_yz;
		doFlag = f_tk00_setco2(&inBlock_yz, &outBlock_yz, conn);
		if (doFlag != 0)
		{
			s.flag = -1;
			return -1;
		}
		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
