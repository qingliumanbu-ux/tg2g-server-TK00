/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:机组碳排情况
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm12_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm12_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");

	CString  sqlstr(""); 
	CString  sg_sign_div("");
	CString  equ_no("");
	CString  gx_div("");

	CDbCommand cmd_inq(conn);

	CModel ttksm02("TTKSM02");

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8); 
		sg_sign_div = bcls_rec->Tables[0].Rows[0]["SG_SIGN_DIV"].ToString();
		equ_no = bcls_rec->Tables[0].Rows[0]["EQU_NO"].ToString();
		gx_div = bcls_rec->Tables[0].Rows[0]["GX_DIV"].ToString();

		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		if (sg_sign_div == "1")
		{
			if (equ_no.Trim() != "" || gx_div.Trim() != "")
			{
				sqlstr = " select st_no,prod_wt,CO2_WT,CO2_WT_UNIT,equ_no,gx_div from ("
					" select st_no,sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,@equ_no as equ_no,@gx_div as gx_div,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (equ_no.Trim() != "")
				{
					sqlstr = sqlstr + " and DEV_CODE_ROUTE like '%'||@equ_no||'%'";
				}
				if (gx_div.Trim() != "")
				{
					sqlstr = sqlstr + " and BACKLOG_EA like '%'||@gx_div||'%'";
				}
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					" union all"
					" select st_no,sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,decode(trim(@equ_no),'',' ','其他') equ_no,decode(trim(@gx_div),'',' ','其他') gx_div,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (equ_no.Trim() != "")
				{
					sqlstr = sqlstr + " and DEV_CODE_ROUTE not like '%'||@equ_no||'%'";
				}
				if (gx_div.Trim() != "")
				{
					sqlstr = sqlstr + " and BACKLOG_EA not like '%'||@gx_div||'%'";
				}
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					" ) order by st_no,gx_div,equ_no"
					;
			}
			else
			{
				sqlstr = " select st_no,prod_wt,CO2_WT,CO2_WT_UNIT from ("
					" select st_no,sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					" ) order by st_no"
					;
			}
		} 
		else
		{
			if (equ_no.Trim() != "" || gx_div.Trim() != "")
			{
				sqlstr = 
					" select sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,@equ_no as equ_no,@gx_div as gx_div,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (equ_no.Trim() != "")
				{
					sqlstr = sqlstr + " and DEV_CODE_ROUTE like '%'||@equ_no||'%'";
				}
				if (gx_div.Trim() != "")
				{
					sqlstr = sqlstr + " and BACKLOG_EA like '%'||@gx_div||'%'";
				}
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					" union all"
					" select sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,decode(trim(@equ_no),'',' ','其他') equ_no,decode(trim(@gx_div),'',' ','其他') gx_div,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (equ_no.Trim() != "")
				{
					sqlstr = sqlstr + " and DEV_CODE_ROUTE not like '%'||@equ_no||'%'";
				}
				if (gx_div.Trim() != "")
				{
					sqlstr = sqlstr + " and BACKLOG_EA not like '%'||@gx_div||'%'";
				}
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					;
			}
			else
			{
				sqlstr = " select sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT,decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(PROD_WT),6)) CO2_WT_UNIT"
					" from ttksm01"
					" where 1=1"
					;
				if (ttksm02["C_DIV"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " and C_DIV=@c_div";
				}
				sqlstr = sqlstr + " and prod_time<=@end_time"
					" and prod_time>=@begin_time"
					" group by st_no"
					;
			}
		}

		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("equ_no", equ_no);
		cmd_inq.Parameters.Set("gx_div", gx_div);
		cmd_inq.Parameters.Set("c_div", ttksm02["C_DIV"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();



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