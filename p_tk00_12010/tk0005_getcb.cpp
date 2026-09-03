/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:通过同步路由获取成本月总消耗 :tacacm2 消耗 tacact7 成本科目
**************************************************/
//框架用头文件
#include "stdafx.h"
#include "epex.h"  //使用同步路由头文件
// service入口
BM2F_ENTERACE(tk0005_getcb)
//-EP_SYSTEM_HEAD_END
int f_tk00_getco2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tk0005_getcb(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CString account_period("");
	CString end_time("");

	CDecimal all_wt = 0;

	CString		mat_type = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel ttk0004("TTK0004");
	CModel ttksm03("TTKSM03");


	CString  sqlstr("");
	CString  sqlstr_temp("");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);

	try
	{
		account_period = bcls_rec->Tables[0].Rows[0]["ACCOUNT_PERIOD"].ToString().SubstringNE(0, 6);
		

		//跨分区取数据再插入
		EIClass inBlock_tk, outBlock_tk;
		inBlock_tk.Tables[0].Columns.Add(DT_STRING, "YEAR");
		inBlock_tk.Tables[0].Columns.Add(DT_STRING, "MONTH");
		inBlock_tk.Tables[0].Rows.Add();
		inBlock_tk.Tables[0].Rows[0]["YEAR"] = account_period.SubstringNE(0,4);
		inBlock_tk.Tables[0].Rows[0]["MONTH"] = account_period.SubstringNE(4, 2);

		Log::Trace("", "", "YEAR={0} MONTH={1}", account_period.SubstringNE(0, 4), account_period.SubstringNE(4, 2));

		f_epex_call_cgi_svc(conn, "TG0RM", "acacb99_tw0", &inBlock_tk, &outBlock_tk, 30);	 //等待30秒
		struct ei_sys s_tmp;		outBlock_tk.GetSYS(&s_tmp);		if (s_tmp.flag < 0)		{			Log::Trace("", "", "调用失败. s.flag={0} s.msg ={1} s.sysmsg={2}", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);		}

		sqlstr = " delete from ttksm03"
			" where ACCOUNT_PERIOD = @account_period"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		for (int i = 0; i < outBlock_tk.Tables[0].Rows.get_Count(); i++)
		{
			//Log::Trace("", "", "wt={0} ", outBlock_tk.Tables[0].Rows[i]["ACT_N"].ToDecimal());
			ttksm03.MergeFrom(outBlock_tk.Tables[0].Rows[i]);
			if (outBlock_tk.Tables[0].Rows[i]["SUM_ACT_N"].ToDecimal() != 0)
			{ 
				ttksm03["REC_CREATOR"] = s.userid;
				ttksm03["REC_CREATE_TIME"] = datetime;
				ttksm03["ACCOUNT_PERIOD"] = account_period;
				ttksm03["COST_CENTER"] = outBlock_tk.Tables[0].Rows[i]["COST_CENTER"].ToString();
				/*if (outBlock_tk.Tables[0].Rows[i]["WCE"].ToString() == "12300")
				{
					outBlock_tk.Tables[0].Rows[i]["WCE"] = "70202";
				}
				if (outBlock_tk.Tables[0].Rows[i]["WCE"].ToString() == "12301")
				{
					outBlock_tk.Tables[0].Rows[i]["WCE"] = "70201";
				}*/
				ttksm03["MAT_CODE_T"] = outBlock_tk.Tables[0].Rows[i]["WCE"].ToString();
				ttksm03["MAT_NAME_T"] = outBlock_tk.Tables[0].Rows[i]["WCE_DESCRIPTION"].ToString();
				ttksm03["AC_WT"] = outBlock_tk.Tables[0].Rows[i]["SUM_ACT_N"].ToDecimal();
				ttksm03.TrimOrBlank();
				if (ttksm03["MAT_CODE_T"].ToString() != "59103") //基础电为流量不参与核算
				{
					ttksm03.Insert();
				}
			}
		}

		//更新成本中心名称
		sqlstr = "update ttksm03 t1 set (COST_CENTER_NAME,FACTORY_ID) = (select COST_CENTER_NAME,FACTORY_ID  from ttk0001c t2 where t1.COST_CENTER=t2.COST_CENTER)"
			" where exists(select COST_CENTER_NAME,FACTORY_ID  from ttk0001c t2 where t1.COST_CENTER=t2.COST_CENTER)"
			" and ACCOUNT_PERIOD=@account_period"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("account_period", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//如果最终消耗有值，则将分摊结果分配到每炉中

		sqlstr = " delete from ttksm02"
			" where 1=1"
			" and HANDLE_DIV = 'F'"
			" and STAT_DATE=@stat_date"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//将辅助成本中心，按产量分摊
		sqlstr = " select  cost_center,case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end,sum(ac_wt) ac_wt"
			" ,nvl((select WHOLE_BACKLOG from ttk0001c t2 where t.cost_center=t2.cost_center),' ') WHOLE_BACKLOG"
			" from ttksm03 t"
			" where 1=1"
			" and cost_center in (select cost_center from ttk0001c where FACTORY_ID = 'LG4' and TYPE='产量')"
			"  and ACCOUNT_PERIOD = @stat_date"
			" group by cost_center, case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end "
			" having sum(ac_wt)!=0"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq_s.SetCommandText(sqlstr);
		cmd_inq_s.Parameters.Set("stat_date", account_period);
		cmd_inq_s.ExecuteReader();
		while (cmd_inq_s.Read())
		{
			sqlstr_temp = "";
			if (cmd_inq_s.GetString(4).Trim() != "")
			{
				sqlstr_temp = " and heat_no in (select heat_no from tmmsmgy06 where @whole_backlog like '%'||dev_code||'%' and stat_date = @stat_date)";
			}
			//根据产量来
			sqlstr = " select sum(MAT_ACT_WT)"
				" from tmmsm56b "
				" where 1=1	"
				;
			sqlstr = sqlstr + sqlstr_temp;
			sqlstr = sqlstr + " and STAT_DATE=@stat_date";	  				
			all_wt = 0;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
			cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
			cmd_inq.Parameters.Set("whole_backlog", cmd_inq_s.GetString(4));
			cmd_inq.Parameters.Set("stat_date", account_period);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				all_wt = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();

			if (all_wt != 0)
			{
				sqlstr = "insert into ttksm02(STAT_DATE, HEAT_NO, ST_NO, MAT_CODE_T,  WT, HANDLE_DIV,cost_center)"
					" SELECT STAT_DATE, HEAT_NO, ST_NO, @mat_code_t,round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6) ,'F',@cost_center "
					" FROM 	tmmsm56b"
					" where 1=1	"
					;
				sqlstr = sqlstr + sqlstr_temp;
				sqlstr = sqlstr + " and STAT_DATE=@stat_date"
					" group by STAT_DATE, HEAT_NO, ST_NO"
					" having round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6)!=0"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.Parameters.Set("all_wt", all_wt);
				cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
				cmd_inq.Parameters.Set("whole_backlog", cmd_inq_s.GetString(4));
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//尾差处理
				sqlstr = " update ttksm02 set wt = wt + (select @use_wt - sum(wt) from ttksm02 where HANDLE_DIV='F' and  mat_code_t=@mat_code_t and cost_center=@cost_center and stat_date=@stat_date)"
					" where HANDLE_DIV = 'F' and  mat_code_t = @mat_code_t and cost_center = @cost_center and stat_date = @stat_date "
					" and rownum = 1"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.Parameters.Set("all_wt", all_wt);
				cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
				cmd_inq.Parameters.Set("whole_backlog", cmd_inq_s.GetString(4));
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
			}

		}
		cmd_inq_s.Close();

	   //消耗项按投入值或是按产量
		sqlstr = " select  cost_center,case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end,sum(ac_wt) ac_wt"
			" from ttksm03 "
			" where 1=1"
			" and cost_center in (select cost_center from ttk0001c where FACTORY_ID = 'LG4'  and TYPE=' ')"
			"  and ACCOUNT_PERIOD = @stat_date"
			" group by cost_center, case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end "
			" having sum(ac_wt)!=0"
			; 
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq_s.SetCommandText(sqlstr);
		cmd_inq_s.Parameters.Set("stat_date", account_period);
		cmd_inq_s.ExecuteReader();
		while (cmd_inq_s.Read())
		{
			sqlstr = " select sum(WT)"
				" from ttksm02 "
				" where 1=1	"
				" and EQU_NO in (select CODE_DESC_5_CONTENT FROM TEP0002 where CODE_DESC_1_CONTENT=@cost_center and CODE_CLASS = 'MMDZ' )"
				" and mat_code_t = @mat_code_t"
				" and STAT_DATE=@stat_date"
				;
			all_wt = 0;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
			cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
			cmd_inq.Parameters.Set("stat_date", account_period);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				all_wt = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close(); 

			if (all_wt != 0 && all_wt != cmd_inq_s.GetDecimal(3))
			{
				sqlstr = "insert into ttksm02(STAT_DATE, HEAT_NO, ST_NO, EQU_NO,MAT_CODE,  WT, HANDLE_DIV,cost_center,MAT_CODE_T)"
					" SELECT STAT_DATE, HEAT_NO, ST_NO, EQU_NO,MAT_CODE,round(SUM(WT)*@use_wt/@all_wt,6) ,'F',@cost_center,@mat_code_t "
					" FROM 	ttksm02"
					" where 1=1	"
					" and EQU_NO in (select CODE_DESC_5_CONTENT FROM TEP0002 where CODE_DESC_1_CONTENT=@cost_center and CODE_CLASS = 'MMDZ' )"
					" and  mat_code_t = @mat_code_t"
					" and STAT_DATE=@stat_date"
					" group by STAT_DATE, HEAT_NO, ST_NO, EQU_NO,MAT_CODE"
					" having round(SUM(WT)*@use_wt/@all_wt,6)!=0"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.Parameters.Set("all_wt", all_wt);
				cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3) - all_wt);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//尾差处理
				sqlstr = " update ttksm02 set wt = wt + (select @use_wt - sum(wt) from ttksm02 where HANDLE_DIV='F' and  mat_code_t=@mat_code_t and cost_center=@cost_center and stat_date=@stat_date)"
					" where HANDLE_DIV = 'F' and  mat_code_t = @mat_code_t and cost_center = @cost_center and stat_date = @stat_date "
					" and rownum = 1"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.Parameters.Set("all_wt", all_wt);
				cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3) - all_wt);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
			}
			else if (all_wt == 0)
			{
				//根据产量来
				sqlstr = " select sum(MAT_ACT_WT)"
					" from tmmsm56b "
					" where 1=1	"
					" and heat_no in (select heat_no from tmmsmgy06 where dev_code in (select CODE_DESC_5_CONTENT FROM TEP0002 where CODE_DESC_1_CONTENT=@cost_center and CODE_CLASS = 'MMDZ' ) and stat_date = @stat_date)"
					" and STAT_DATE=@stat_date"
					;
				all_wt = 0;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					all_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				if (all_wt != 0)
				{
					sqlstr = "insert into ttksm02(STAT_DATE, HEAT_NO, ST_NO, MAT_CODE_T,  WT, HANDLE_DIV,cost_center)"
						" SELECT STAT_DATE, HEAT_NO, ST_NO, @mat_code_t,round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6) ,'F',@cost_center "
						" FROM 	tmmsm56b"
						" where 1=1	"
						" and heat_no in (select heat_no from tmmsmgy06 where dev_code in (select CODE_DESC_5_CONTENT FROM TEP0002 where CODE_DESC_1_CONTENT=@cost_center and CODE_CLASS = 'MMDZ' ) and stat_date = @stat_date)"
						" and STAT_DATE=@stat_date"
						" group by STAT_DATE, HEAT_NO, ST_NO"
						" having  round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6)!=0"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
					cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
					cmd_inq.Parameters.Set("stat_date", account_period);
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					//尾差处理
					sqlstr = " update ttksm02 set wt = wt + (select @use_wt - sum(wt) from ttksm02 where HANDLE_DIV='F' and  mat_code_t=@mat_code_t and cost_center=@cost_center and stat_date=@stat_date)"
						" where HANDLE_DIV = 'F' and  mat_code_t = @mat_code_t and cost_center = @cost_center and stat_date = @stat_date "
						" and rownum = 1"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
					cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
					cmd_inq.Parameters.Set("stat_date", account_period);
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
			}

			}
			cmd_inq_s.Close();


			//回收的放入虚拟消耗，按物料进行分摊

			sqlstr = " select  cost_center,case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end,sum(ac_wt) ac_wt"
				" from ttksm03 t"
				" where 1=1"
				" and cost_center in (select cost_center from ttk0001c where FACTORY_ID = 'LG4' and TYPE='物料')"
				"  and ACCOUNT_PERIOD = @stat_date"
				" group by cost_center, case when mat_code_t in ('59100','59101','59102') then '59103' else mat_code_t end "
				" having sum(ac_wt)!=0"
				;
			Log::Trace("", "", "sqlstr={0} ", sqlstr);
			cmd_inq_s.SetCommandText(sqlstr);
			cmd_inq_s.Parameters.Set("stat_date", account_period);
			cmd_inq_s.ExecuteReader();
			while (cmd_inq_s.Read())
			{
				sqlstr_temp = "";
				if (cmd_inq_s.GetString(2)== "18100") //主原料回收-废钢-碳素废钢-碳钢
				{
					sqlstr_temp = " and substr(st_no,1,1) in ('1','4')";
				}
				if (cmd_inq_s.GetString(2) == "18101") //主原料回收-废钢-铬不锈废钢-不锈钢
				{
					sqlstr_temp = " and substr(st_no,2,1) in ('F','M')";
				}
				if (cmd_inq_s.GetString(2) == "18102") //主原料回收-废钢-镍不锈废钢-不锈钢
				{
					sqlstr_temp = " and substr(st_no,2,1) in ('A','D')";
				}
				if (cmd_inq_s.GetString(2) == "18103") //主原料回收-废钢-镍基合金废钢-不锈钢
				{
					sqlstr_temp = " and substr(st_no,1,1) in ('2','3','5')";
				}
				//根据产量来
				sqlstr = " select sum(MAT_ACT_WT)"
					" from tmmsm56b "
					" where 1=1	"
					;
				sqlstr = sqlstr + sqlstr_temp;
				sqlstr = sqlstr + " and STAT_DATE=@stat_date";
				all_wt = 0;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
				cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
				cmd_inq.Parameters.Set("stat_date", account_period);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					all_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				if (all_wt != 0)
				{
					sqlstr = "insert into ttksm02(STAT_DATE, HEAT_NO, ST_NO, MAT_CODE_T,  WT, HANDLE_DIV,cost_center)"
						" SELECT STAT_DATE, HEAT_NO, ST_NO, @mat_code_t,round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6) ,'F',@cost_center "
						" FROM 	tmmsm56b"
						" where 1=1	"
						;
					sqlstr = sqlstr + sqlstr_temp;
					sqlstr = sqlstr + " and STAT_DATE=@stat_date"
						" group by STAT_DATE, HEAT_NO, ST_NO"
						" having round(SUM(MAT_ACT_WT)*@use_wt/@all_wt,6)!=0"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
					cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
					cmd_inq.Parameters.Set("stat_date", account_period);
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					//尾差处理
					sqlstr = " update ttksm02 set wt = wt + (select @use_wt - sum(wt) from ttksm02 where HANDLE_DIV='F' and  mat_code_t=@mat_code_t and cost_center=@cost_center and stat_date=@stat_date)"
						" where HANDLE_DIV = 'F' and  mat_code_t = @mat_code_t and cost_center = @cost_center and stat_date = @stat_date "
						" and rownum = 1"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code_t", cmd_inq_s.GetString(2));
					cmd_inq.Parameters.Set("cost_center", cmd_inq_s.GetString(1));
					cmd_inq.Parameters.Set("stat_date", account_period);
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("use_wt", cmd_inq_s.GetDecimal(3));
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}

			}
			cmd_inq_s.Close();

			

			//更新物料代码为空的物料编码
			sqlstr = " update ttksm02 t set (mat_code,mat_name) =(select mat_code,mat_name from ttk0001 t2 where t2.mat_code_t = t.mat_code_t and rownum=1)"
				"  where 1=1  "
				" and EXISTS (select 1 from ttk0001 t2 where t2.mat_code_t = t.mat_code_t)"
				" and mat_code =' '"
				//" and HANDLE_DIV = 'F'"
				" and stat_date = @stat_date "
				;
			Log::Trace("", "", "sqlstr={0} ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", account_period);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新物料代码为空的物料编码
			sqlstr = " update ttksm02 t set  mat_name  =(select mat_name from ttk0001 t2 where t2.mat_code = t.mat_code)"
				"  where 1=1  "
				" and EXISTS (select 1 from ttk0001 t2 where t2.mat_code = t.mat_code)"
				" and stat_date = @stat_date "
				;
			Log::Trace("", "", "sqlstr={0} ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", account_period);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新ttksm02的排放因子
			EIClass inBlock_yz, outBlock_yz;
			inBlock_yz.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
			inBlock_yz.Tables[0].Columns.Add(DT_STRING, "DATA_TYPE");
			inBlock_yz.Tables[0].Columns.Add(DT_STRING, "VALID_TIME");
			inBlock_yz.Tables[0].Rows.Add();
			sqlstr = " select distinct mat_code from ttksm02"
				" where 1=1"
				" and stat_date=@stat_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", account_period);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				inBlock_yz.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
				doFlag = f_tk00_getco2(&inBlock_yz, &outBlock_yz, conn);
				if (doFlag != 0)
				{
					s.flag = -1;
					return -1;
				}
				if (outBlock_yz.Tables[0].Rows.get_Count() > 0)
				{
					Log::Trace("", "", "mat_code={0} ", cmd_inq.GetString(1));
					ttk0004.MergeFrom(outBlock_yz.Tables[0].Rows[0]);
					sqlstr = " update ttksm02 set CO2_COE= @co2_coe"
						",CO2_WT = round(wt*@co2_coe,6)"
						" ,CO2_COE_UNIT = @co2_coe_unit"
						",CO2_COE1= @co2_coe1"
						",CO2_WT1 = round(wt*@co2_coe1,6)"
						",CO2_COE2= @co2_coe2"
						",CO2_WT2 = round(wt*@co2_coe2,6)"
						" where 1=1"
						" and mat_code = @mat_code"
						" and stat_date=@stat_date"
						;
					cmd_inq_s.SetCommandText(sqlstr);
					cmd_inq_s.Parameters.Set("stat_date", account_period);
					cmd_inq_s.Parameters.Set("mat_code", cmd_inq.GetString(1));
					cmd_inq_s.Parameters.Set("mat_name", ttk0004["MAT_NAME"].ToString());
					cmd_inq_s.Parameters.Set("co2_coe", ttk0004["CO2_COE"].ToDecimal());
					cmd_inq_s.Parameters.Set("co2_coe_unit", ttk0004["CO2_COE_UNIT"].ToString());
					cmd_inq_s.Parameters.Set("co2_coe1", ttk0004["CO2_COE1"].ToDecimal());
					cmd_inq_s.Parameters.Set("co2_coe2", ttk0004["CO2_COE2"].ToDecimal());
					cmd_inq_s.ExecuteNonQuery();
					cmd_inq_s.Close();
				}
			}
			cmd_inq.Close(); 		
		
		


		//更新钢水重量
		sqlstr = " update ttksm02 t1 set (C_DIV,prod_time,BACKLOG_EA) = (select C_DIV,prod_time,BACKLOG_EA from ttksm01 t2 where t1.heat_no =t2.heat_no)"
			" where exists (select 1 from ttksm01 t2 where t1.heat_no =t2.heat_no)"
			" and HANDLE_DIV = 'F'"
			" and stat_date=@stat_date"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", account_period);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新  ttksm01的碳排量
		sqlstr = " update ttksm01 t1 set (CO2_WT,CO2_WT1,CO2_WT2) = (select SUM(CO2_WT),SUM(CO2_WT1),SUM(CO2_WT2) from ttksm02 t2 where t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (SELECT 1 FROM ttksm02 t2 where  t1.heat_no=t2.heat_no)"
			" and STAT_DATE=@stat_date"
			;
		Log::Trace("", "", "sqlstr={0} ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", account_period);
		cmd_inq.ExecuteNonQuery();
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