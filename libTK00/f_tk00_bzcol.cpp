/*=========================================================================
//程序名称:     f_tk00_getsm
//隶属子系统:   TK
//产品名称:     BM2PES
//创建人员:     ZHOULI
//创建时间:     2012-11-26
//修改人员:   获取生产实绩信息  
//修改日期:     
//=========================================================================*/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
BM2_FUNCTION_EXPORT
int f_tk00_bzco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk00_bzcol(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;

	CString sqlstr = "";
	CString stat_date_s = CDateTime::Now().AddMonths(-7).ToString("yyyyMM");
	CString stat_date_e = CDateTime::Now().AddMonths(-1).ToString("yyyyMM");
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString st_no = "";
	CString type_desc = "";
	CModel ttk0004("TTK0004");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{  

		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE_S"))
		{
			stat_date_s = bcls_rec->Tables[0].Rows[0]["STAT_DATE_S"].ToString().SubstringNE(0, 6);
		}		
		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE_E"))
		{
			stat_date_e = bcls_rec->Tables[0].Rows[0]["STAT_DATE_E"].ToString().SubstringNE(0, 6);
		}
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
		{
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		}

		stat_date_s = "202405";
		stat_date_e = "202502";
		type_desc = bcls_rec->Tables[0].Rows[0]["TYPE_DESC"].ToString();

		if (type_desc == "工序费" || type_desc.Trim() == "")
		{
			sqlstr = " delete from ttk0006 "
				" where  1=1"
				" and TYPE_DESC = '工序费'"
			;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date_e", stat_date_e);
			cmd_inq.Parameters.Set("stat_date_s", stat_date_s);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into ttk0006 (REC_CREATOR,REC_CREATE_TIME,TYPE_DESC,SUB_BACKLOG_CODE,MAT_CODE,MAT_NAME,WT)"
				" select @rec_creator,@rec_create_time,'工序费',t.SUB_BACKLOG_CODE,t3.MAT_CODE,t3.MAT_NAME,round(AC_WT/mat_act_wt,6)"
				" from ("
				" select t1.MAT_CODE_T,sum(AC_WT) AC_WT,t2.SUB_BACKLOG_CODE"
				",(select sum(mat_act_wt) mat_act_wt from tmmsm56b where stat_date <= @stat_date_e and stat_date >= @stat_date_s) mat_act_wt"
				" from ttksm03 t1"
				" left join ttk0001c t2 on t1.COST_CENTER =t2.COST_CENTER and t2.FACTORY_ID='LG4'" 				
				" where 1=1"
				" and nvl(t2.SUB_BACKLOG_CODE,' ')!=' '"
				" and t1.account_period <=@stat_date_e"
				" and t1.account_period >=@stat_date_s"
				" group by t1.MAT_CODE_T,t2.SUB_BACKLOG_CODE"
				") t"
				" left join ttk0001 t3 on t.mat_code_t = t3.mat_code_t	 "	
				" where t3.DEFAULT_FLAG = '1'"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date_e", stat_date_e);
			cmd_inq.Parameters.Set("stat_date_s", stat_date_s);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}

		/*if (type_desc == "主原料" || type_desc.Trim() == "")
		{
			sqlstr = " delete from ttk0006 "
				" where  1=1"
				" and st_no in (select st_no from tmmsm56b where  stat_date <= @stat_date_e and stat_date >= @stat_date_s)"
				" and TYPE_DESC = '主原料'"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date_e", stat_date_e);
			cmd_inq.Parameters.Set("stat_date_s", stat_date_s);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into ttk0006 (REC_CREATOR,REC_CREATE_TIME,TYPE_DESC,st_no,MAT_CODE,MAT_NAME,WT)"
				" select @rec_creator,@rec_create_time,'主原料',t.st_no,t.MAT_CODE,t3.MAT_NAME,round(AC_WT/mat_act_wt,6)"
				" from ("
				" select t1.MAT_CODE,sum(wt) AC_WT,ST_NO"				
				" from ttksm02 t1"
				" where 1=1"
				" and EQU_NO!=' '"
				" and t1.stat_date <=@stat_date_e"
				" and t1.stat_date >=@stat_date_s"
				" group by MAT_CODE,ST_NO"
				") t"
				" left join ttk0001 t3 on t.mat_code = t3.mat_code	 "
				" left join  (select ST_NO, sum(mat_act_wt) mat_act_wt from tmmsm56b where stat_date <= @stat_date_e and stat_date >= @stat_date_s group by st_no) t2 on t.st_no=t2.st_no"					
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date_e", stat_date_e);
			cmd_inq.Parameters.Set("stat_date_s", stat_date_s);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}*/
		
		
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		doFlag = f_tk00_bzco2(&bcls_rec_xh, &bcls_ret_xh, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.msg, (const char*)str, sizeof(s.msg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
