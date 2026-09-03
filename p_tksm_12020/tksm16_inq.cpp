/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:机组碳排趋势图
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm16_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm16_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CDecimal mat_act_wt = 0;
	int i = 0;

	CString		dev_code = "IF";

	CString prod_date = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");


	CString  sqlstr("");
	CDbCommand cmd_inq(conn);

	try
	{
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "日碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "累计碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "日碳排量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "累计碳排量");

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "甲日碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "甲累计碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "甲日碳排量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "甲累计碳排量");

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "乙日碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "乙累计碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "乙日碳排量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "乙累计碳排量");

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丙日碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丙累计碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丙日碳排量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丙累计碳排量");

		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丁日碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丁累计碳强度");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丁日碳排量");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "丁累计碳排量");
		bcls_ret->Tables[0].Rows.Add();	


		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].set_TableName("日趋势");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "碳强度");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "直排强度");
		
		


		bcls_ret->Tables.Add();
		bcls_ret->Tables[2].set_TableName("产量趋势");
		bcls_ret->Tables[2].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[2].Columns.Add(DT_DECIMAL, "产量");
		bcls_ret->Tables[2].Rows.Add();
		


		bcls_ret->Tables.Add();
		bcls_ret->Tables[3].set_TableName("甲日趋势");
		bcls_ret->Tables[3].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[3].Columns.Add(DT_DECIMAL, "碳强度");
		bcls_ret->Tables[3].Columns.Add(DT_DECIMAL, "直排强度");
	

		bcls_ret->Tables.Add();
		bcls_ret->Tables[4].set_TableName("甲产量趋势");
		bcls_ret->Tables[4].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[4].Columns.Add(DT_DECIMAL, "产量");
		

		bcls_ret->Tables.Add();
		bcls_ret->Tables[5].set_TableName("乙日趋势");
		bcls_ret->Tables[5].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[5].Columns.Add(DT_DECIMAL, "碳强度");
		bcls_ret->Tables[5].Columns.Add(DT_DECIMAL, "直排强度");
		

		bcls_ret->Tables.Add();
		bcls_ret->Tables[6].set_TableName("乙产量趋势");
		bcls_ret->Tables[6].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[6].Columns.Add(DT_DECIMAL, "产量");

		bcls_ret->Tables.Add();
		bcls_ret->Tables[7].set_TableName("丙日趋势");
		bcls_ret->Tables[7].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[7].Columns.Add(DT_DECIMAL, "碳强度");
		bcls_ret->Tables[7].Columns.Add(DT_DECIMAL, "直排强度");
		

		bcls_ret->Tables.Add();
		bcls_ret->Tables[8].set_TableName("丙产量趋势");
		bcls_ret->Tables[8].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[8].Columns.Add(DT_DECIMAL, "产量");

		bcls_ret->Tables.Add();
		bcls_ret->Tables[9].set_TableName("丁日趋势");
		bcls_ret->Tables[9].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[9].Columns.Add(DT_DECIMAL, "碳强度");
		bcls_ret->Tables[9].Columns.Add(DT_DECIMAL, "直排强度");
		

		bcls_ret->Tables.Add();
		bcls_ret->Tables[10].set_TableName("丁产量趋势");
		bcls_ret->Tables[10].Columns.Add(DT_STRING, "日期");
		bcls_ret->Tables[10].Columns.Add(DT_DECIMAL, "产量");



		dev_code = bcls_rec->Tables[0].Rows[0]["DEE_CODE"].ToString();
		//if (dev_code == "IF") //中频炉
		{
			sqlstr = " select case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt_all=0 then 0 else round(CO2_WT_ALL/prod_wt_all,4) end"
				" ,round(CO2_WT,2)"
				" ,round(CO2_WT_ALL,2)"
				",case when prod_wt_a=0 then 0 else round(CO2_WT_a/prod_wt_a,4) end"
				",case when prod_wt_all_a=0 then 0 else round(CO2_WT_ALL_a/prod_wt_all_a,4) end"
				" ,round(CO2_WT_a,2)"
				" ,round(CO2_WT_ALL_a,2)"
				",case when prod_wt_b=0 then 0 else round(CO2_WT_b/prod_wt_b,4) end"
				",case when prod_wt_all_b=0 then 0 else round(CO2_WT_ALL_b/prod_wt_all_b,4) end"
				" ,round(CO2_WT_b,2)"
				" ,round(CO2_WT_ALL_b,2)"
				",case when prod_wt_c=0 then 0 else round(CO2_WT_c/prod_wt_c,4) end"
				",case when prod_wt_all_c=0 then 0 else round(CO2_WT_ALL_c/prod_wt_all_c,4) end"
				" ,round(CO2_WT_c,2)"
				" ,round(CO2_WT_ALL_c,2)"
				",case when prod_wt_d=0 then 0 else round(CO2_WT_d/prod_wt_d,4) end"
				",case when prod_wt_all_d=0 then 0 else round(CO2_WT_ALL_d/prod_wt_all_d,4) end"
				" ,round(CO2_WT_d,2)"
				" ,round(CO2_WT_ALL_d,2)"
				" from "
				" (select sum(ACTRESULT) prod_wt_all,sum(case when START_TIME like @prod_date||'%' then ACTRESULT else 0 end) prod_wt"
				", sum(case when PROD_SHIFT_GROUP ='A' then ACTRESULT else 0 end) prod_wt_all_a"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='A'  then ACTRESULT else 0 end) prod_wt_a"
				", sum(case when PROD_SHIFT_GROUP ='B' then ACTRESULT else 0 end) prod_wt_all_b"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='B'  then ACTRESULT else 0 end) prod_wt_b"
				", sum(case when PROD_SHIFT_GROUP ='C' then ACTRESULT else 0 end) prod_wt_all_c"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='C'  then ACTRESULT else 0 end) prod_wt_c"
				", sum(case when PROD_SHIFT_GROUP ='D' then ACTRESULT else 0 end) prod_wt_all_d"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='D'  then ACTRESULT else 0 end) prod_wt_d"
				" from tmmsm19"
				" where START_TIME <=@end_time"
				" and START_TIME >= @begin_time ) t1"
				" ,(select sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT_ALL"
				" ,sum(case when START_TIME like @prod_date || '%' then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT"
				", sum(case when PROD_SHIFT_GROUP ='A' then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_ALL_A"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='A'  then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_A"
				", sum(case when PROD_SHIFT_GROUP ='B' then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_ALL_B"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='B'  then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_B"
				", sum(case when PROD_SHIFT_GROUP ='C' then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_ALL_C"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='C'  then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_C"
				", sum(case when PROD_SHIFT_GROUP ='D' then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_ALL_D"
				", sum(case when START_TIME like @prod_date || '%' and PROD_SHIFT_GROUP ='D'  then DEVO_WT/1000*nvl(CO2_COE,0) else 0 end) CO2_WT_D"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" ) t2"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["日碳强度"] = cmd_inq.GetDecimal(1);
				bcls_ret->Tables[0].Rows[0]["累计碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[0].Rows[0]["日碳排量"] = cmd_inq.GetDecimal(3);
				bcls_ret->Tables[0].Rows[0]["累计碳排量"] = cmd_inq.GetDecimal(4);
				bcls_ret->Tables[0].Rows[0]["甲日碳强度"] = cmd_inq.GetDecimal(5);
				bcls_ret->Tables[0].Rows[0]["甲累计碳强度"] = cmd_inq.GetDecimal(6);
				bcls_ret->Tables[0].Rows[0]["甲日碳排量"] = cmd_inq.GetDecimal(7);
				bcls_ret->Tables[0].Rows[0]["甲累计碳排量"] = cmd_inq.GetDecimal(8);
				bcls_ret->Tables[0].Rows[0]["乙日碳强度"] = cmd_inq.GetDecimal(9);
				bcls_ret->Tables[0].Rows[0]["乙累计碳强度"] = cmd_inq.GetDecimal(10);
				bcls_ret->Tables[0].Rows[0]["乙日碳排量"] = cmd_inq.GetDecimal(11);
				bcls_ret->Tables[0].Rows[0]["乙累计碳排量"] = cmd_inq.GetDecimal(12);
				bcls_ret->Tables[0].Rows[0]["丙日碳强度"] = cmd_inq.GetDecimal(13);
				bcls_ret->Tables[0].Rows[0]["丙累计碳强度"] = cmd_inq.GetDecimal(14);
				bcls_ret->Tables[0].Rows[0]["丙日碳排量"] = cmd_inq.GetDecimal(15);
				bcls_ret->Tables[0].Rows[0]["丙累计碳排量"] = cmd_inq.GetDecimal(16);
				bcls_ret->Tables[0].Rows[0]["丁日碳强度"] = cmd_inq.GetDecimal(17);
				bcls_ret->Tables[0].Rows[0]["丁累计碳强度"] = cmd_inq.GetDecimal(18);
				bcls_ret->Tables[0].Rows[0]["丁日碳排量"] = cmd_inq.GetDecimal(19);
				bcls_ret->Tables[0].Rows[0]["丁累计碳排量"] = cmd_inq.GetDecimal(20);
			}
			cmd_inq.Close();

			//日趋势图
			sqlstr = " select t1.prod_date"
				",case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt = 0 then 0 else round(CO2_WT1 / prod_wt, 4) end"
				" from "
				" (select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)) t1"
				" left join "
				" (select substr(t1.START_TIME,1,8) prod_date,sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT"
				", sum(DEVO_WT / 1000 * nvl(CO2_COE1, 0)) CO2_WT1"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" group by substr(t1.START_TIME,1,8) ) t2"
				" on t1.prod_date = t2.prod_date"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[1].Rows.Add();
				bcls_ret->Tables[1].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[1].Rows[i]["碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[1].Rows[i]["直排强度"] = cmd_inq.GetDecimal(3);
				i++;
			} 
			cmd_inq.Close();

			//产量趋势图
			sqlstr = "select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[2].Rows.Add();
				bcls_ret->Tables[2].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[2].Rows[i]["产量"] = cmd_inq.GetDecimal(2);
				i++;
			}
			cmd_inq.Close();

			//甲班
			//日趋势图
			sqlstr = " select t1.prod_date"
				",case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt = 0 then 0 else round(CO2_WT1 / prod_wt, 4) end"
				" from "
				" (select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where 1=1"
				" and PROD_SHIFT_GROUP = 'A'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)) t1"
				" left join "
				" (select substr(t1.START_TIME,1,8) prod_date,sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT"
				", sum(DEVO_WT / 1000 * nvl(CO2_COE1, 0)) CO2_WT1"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where 1=1"
				"  and t1.PROD_SHIFT_GROUP = 'A'"
				" and t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" group by substr(t1.START_TIME,1,8) ) t2"
				" on t1.prod_date = t2.prod_date"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[3].Rows.Add();
				bcls_ret->Tables[3].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[3].Rows[i]["碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[3].Rows[i]["直排强度"] = cmd_inq.GetDecimal(3);
				i++;
			}
			cmd_inq.Close();

			//产量趋势图
			sqlstr = "select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where  1=1"
				" and PROD_SHIFT_GROUP = 'A'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[4].Rows.Add();
				bcls_ret->Tables[4].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[4].Rows[i]["产量"] = cmd_inq.GetDecimal(2);
				i++;
			}
			cmd_inq.Close();

			//乙班
			//日趋势图
			sqlstr = " select t1.prod_date"
				",case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt = 0 then 0 else round(CO2_WT1 / prod_wt, 4) end"
				" from "
				" (select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where 1=1"
				" and PROD_SHIFT_GROUP = 'A'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)) t1"
				" left join "
				" (select substr(t1.START_TIME,1,8) prod_date,sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT"
				", sum(DEVO_WT / 1000 * nvl(CO2_COE1, 0)) CO2_WT1"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where 1=1"
				"  and t1.PROD_SHIFT_GROUP = 'B'"
				" and t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" group by substr(t1.START_TIME,1,8) ) t2"
				" on t1.prod_date = t2.prod_date"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[5].Rows.Add();
				bcls_ret->Tables[5].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[5].Rows[i]["碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[5].Rows[i]["直排强度"] = cmd_inq.GetDecimal(3);
				i++;
			}
			cmd_inq.Close();

			//产量趋势图
			sqlstr = "select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where  1=1"
				" and PROD_SHIFT_GROUP = 'B'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[6].Rows.Add();
				bcls_ret->Tables[6].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[6].Rows[i]["产量"] = cmd_inq.GetDecimal(2);
				i++;
			}
			cmd_inq.Close();

			//丙班
			//日趋势图
			sqlstr = " select t1.prod_date"
				",case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt = 0 then 0 else round(CO2_WT1 / prod_wt, 4) end"
				" from "
				" (select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where 1=1"
				" and PROD_SHIFT_GROUP = 'C'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)) t1"
				" left join "
				" (select substr(t1.START_TIME,1,8) prod_date,sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT"
				", sum(DEVO_WT / 1000 * nvl(CO2_COE1, 0)) CO2_WT1"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where 1=1"
				"  and t1.PROD_SHIFT_GROUP = 'C'"
				" and t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" group by substr(t1.START_TIME,1,8) ) t2"
				" on t1.prod_date = t2.prod_date"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[7].Rows.Add();
				bcls_ret->Tables[7].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[7].Rows[i]["碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[7].Rows[i]["直排强度"] = cmd_inq.GetDecimal(3);
				i++;
			}
			cmd_inq.Close();

			//产量趋势图
			sqlstr = "select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where  1=1"
				" and PROD_SHIFT_GROUP = 'C'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[8].Rows.Add();
				bcls_ret->Tables[8].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[8].Rows[i]["产量"] = cmd_inq.GetDecimal(2);
				i++;
			}
			cmd_inq.Close();

			//丁班
			//日趋势图
			sqlstr = " select t1.prod_date"
				",case when prod_wt = 0 then 0 else round(CO2_WT / prod_wt, 4) end"
				",case when prod_wt = 0 then 0 else round(CO2_WT1 / prod_wt, 4) end"
				" from "
				" (select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where 1=1"
				" and PROD_SHIFT_GROUP = 'A'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)) t1"
				" left join "
				" (select substr(t1.START_TIME,1,8) prod_date,sum(DEVO_WT/1000*nvl(CO2_COE,0)) CO2_WT"
				", sum(DEVO_WT / 1000 * nvl(CO2_COE1, 0)) CO2_WT1"
				" from  tmmsm19 t1"
				" left join tmmsm2a_yl t2 on t1.proc_no=t2.l2_proc_no"
				" left join "
				" (select t.mat_code,(case when t2.COE =0 then 1 else COE end)*t.CO2_COE as CO2_COE"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE1 as CO2_COE1"
				",(case when t2.COE =0 then 1 else COE end)*t.CO2_COE2 as CO2_COE2"
				" from TTK0004 t"
				" left join ttk0001 t2 on t.mat_code = t2.mat_code"
				" where (t.mat_code,t.VALID_TIME) in (select mat_code,max(VALID_TIME) from TTK0004 group by mat_code)"
				" ) t3 on t2.mat_code=t3.mat_code"
				" where 1=1"
				"  and t1.PROD_SHIFT_GROUP = 'B'"
				" and t1.START_TIME <=@end_time"
				" and t1.START_TIME >= @begin_time"
				" group by substr(t1.START_TIME,1,8) ) t2"
				" on t1.prod_date = t2.prod_date"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[9].Rows.Add();
				bcls_ret->Tables[9].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[9].Rows[i]["碳强度"] = cmd_inq.GetDecimal(2);
				bcls_ret->Tables[9].Rows[i]["直排强度"] = cmd_inq.GetDecimal(3);
				i++;
			}
			cmd_inq.Close();

			//产量趋势图
			sqlstr = "select substr(START_TIME,1,8) prod_date,sum(ACTRESULT) prod_wt"
				" from tmmsm19"
				" where  1=1"
				" and PROD_SHIFT_GROUP = 'B'"
				" and START_TIME <=@end_time"
				" and START_TIME >= @begin_time "
				" group by substr(START_TIME,1,8)"
				" order by prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", prod_date.Substring(0, 6) + "01");
			cmd_inq.Parameters.Set("end_time", prod_date + "235959");
			cmd_inq.Parameters.Set("prod_date", prod_date);
			cmd_inq.ExecuteReader();
			i = 0;
			while (cmd_inq.Read())
			{
				bcls_ret->Tables[10].Rows.Add();
				bcls_ret->Tables[10].Rows[i]["日期"] = cmd_inq.GetString(1);
				bcls_ret->Tables[10].Rows[i]["产量"] = cmd_inq.GetDecimal(2);
				i++;
			}
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