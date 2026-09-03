/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳控排组成
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm11_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm11_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString begin_time("");
	CString end_time("");
	CString st_no("");

	CString		mat_type = " ";

	CModel ttksm02("TTKSM02");


	CString  sqlstr("");


	CDbCommand cmd_inq(conn);

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();  
		ttksm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr = " select t1.st_no,t1.backlog_ea,t1.prod_wt,c_wt_sj,c_wt_max,c_wt_min,round(nvl(t2.CO2_WT,0),4) as C_WT_BZ"
			",nvl(t3.GRADE_TYPE1,' ') GRADE_TYPE1,nvl(t3.GRADE_TYPE2,' ') GRADE_TYPE2,nvl(t3.GRADE_TYPE3,' ') GRADE_TYPE3,nvl(t3.GRADE_TYPE4,' ') GRADE_TYPE4,nvl(t3.GRADE_DESC,' ') GRADE_DESC"
			" from "
			" ("
			" select st_no,backlog_ea,sum(prod_wt) prod_wt"
			",decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(prod_wt),4) ) c_wt_sj"
			",max(wt_c_unit) c_wt_max,min(wt_c_unit) c_wt_min"
			" from ("
			" select st_no,backlog_ea,prod_time,sum(prod_wt) prod_wt,sum(CO2_WT) CO2_WT "
			",decode(sum(prod_wt),0,0,round(sum(CO2_WT)/sum(prod_wt),4) ) wt_c_unit"
			" from ttksm01"
			" where prod_time<=@end_time"
			" and prod_time>=@begin_time"
			;
		if (st_no.Trim() != "")
			sqlstr = sqlstr + " and st_no = @st_no";
		if (ttksm02["C_DIV"].ToString().Trim() != "")
		{
			sqlstr = sqlstr + " and C_DIV=@c_div";
		}
		sqlstr = sqlstr + " group by st_no,backlog_ea,prod_time )"
			" group by st_no,backlog_ea"
			" ) t1 left join ttk0005 t2 on t1.st_no =t2.st_no and t1.backlog_ea = t2.WHOLE_BACKLOG "
			" left join tQMTSB10 t3 on t1.st_no=t3.STEEL_GRADE"
			" order by st_no"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("st_no", st_no);
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