/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  
// service入口
BM2F_ENTERACE(tksm10_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm10_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString		proc_div = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel ttksm11("TTKSM11");
	CModel ttksm12("TTKSM12");
	CModel ttksm13("TTKSM13");
	CString  sqlstr("");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"];
		ttksm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "proc_div=[{0}] ", proc_div);
		Log::Trace("", __FUNCTION__, "ST_NO=[{0}] ", ttksm11["ST_NO"].ToString());
		Log::Trace("", __FUNCTION__, "WHOLE_BACKLOG=[{0}] ", ttksm11["WHOLE_BACKLOG"].ToString());
		//主信息
		if (proc_div == "M")
		{
			sqlstr = 
				" SELECT t1.* "
				",nvl((SELECT SUM(CO2_WT) CO2_WT_SUM  FROM TTKSM12 t2 where t2.seq_no=t1.seq_no),0) CO2_WT_SUM"
				" FROM TTKSM11 t1 "
				;
			if (ttksm11["WHOLE_BACKLOG"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND  t1.WHOLE_BACKLOG like @WHOLE_BACKLOG||'%'";

			}
			if (ttksm11["ST_NO"].ToString().Trim() != "")
			{
				sqlstr = sqlstr + " AND  t1.ST_NO like @ST_NO||'%'";

			}
			
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}] ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("ST_NO", ttksm11["ST_NO"].ToString());
			cmd_inq.Parameters.Set("WHOLE_BACKLOG", ttksm11["WHOLE_BACKLOG"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (proc_div == "S")
		{
			bcls_ret->Tables.Add();
			sqlstr = "SELECT *  FROM ttksm12  "
				" WHERE 1=1"
				" and SEQ_NO=@seq_no"
				 " ORDER BY SEQ_NO, SEQ_NO_RECIPE"
				 ;
			cmd_inq.Parameters.Set("seq_no", ttksm11["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}] ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " SELECT *  FROM ttksm13 "
				" WHERE 1=1"
				" and SEQ_NO_RECIPE in (select SEQ_NO_RECIPE from ttksm12 where SEQ_NO=@seq_no )"
				" ORDER BY SEQ_NO_RECIPE,mat_code"
				;
			cmd_inq.Parameters.Set("seq_no", ttksm11["SEQ_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();
			 
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}