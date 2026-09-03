/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2023-09-22
Description:碳排跟踪
**************************************************/
//框架用头文件
#include "stdafx.h"
// service入口
BM2F_ENTERACE(tksm15_inq)
//-EP_SYSTEM_HEAD_END

int f_tksm15_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount = 0;

	CDecimal mat_act_wt = 0;
	CDecimal amount_tax = 0;

	CString		mat_type = " ";

	CString prod_date = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");


	CString  sqlstr("");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{	

		//prod_date = "20250226";
		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "日产量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "累计产量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "日成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "累计成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "日碳排量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "累计碳排量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "日碳成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "累计碳成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢日产量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢累计产量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢日成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢累计成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢日碳排量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢累计碳排量");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢日碳成本");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "不锈钢累计碳成本");
		bcls_ret->Tables[1].Rows.Add();

		sqlstr = " select AMOUNT_TAX"
			" from ttk0002"
			" order by VALID_TIME desc"
			;
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			amount_tax = cmd_inq1.GetDecimal(1);
		}
		cmd_inq1.Close();

		sqlstr = " select sum(prod_wt1),sum(prod_wt2),sum(cost1),sum(cost2)"
			",sum(prod_wt3),sum(prod_wt4),sum(cost3),sum(cost4)"
			" from ("
			" select sum(case when aod_bof_e_dtime=@prod_date and substr(GRADE_ID,1,1) in ('2','3','5') then MAT_ACT_WT else 0 end) prod_wt1"
			",sum(case when substr(GRADE_ID,1,1) in ('2','3','5') then MAT_ACT_WT else 0 end) prod_wt2"
			",sum(case when aod_bof_e_dtime = @prod_date and substr(GRADE_ID, 1, 1) not in ('2', '3', '5') then MAT_ACT_WT else 0 end) prod_wt3"
			",sum(case when substr(GRADE_ID,1,1) not in ('2','3','5') then MAT_ACT_WT else 0 end) prod_wt4"
			",0 cost1,0 cost2,0 cost3,0 cost4"
			" from tqmtscb02_mx"
			" where DATE_C = @stat_date"
			" union all"
			" select 0 prod_wt1,0 prod_wt2,0 prod_wt3,0 prod_wt4,sum(case when aod_bof_e_dtime=@prod_date and substr(GRADE_ID,1,1) in ('2','3','5') then cost else 0 end) cost1"
			",sum(case when substr(GRADE_ID,1,1) in ('2','3','5') then cost else 0 end) cost2"
			", sum(case when aod_bof_e_dtime = @prod_date and substr(GRADE_ID, 1, 1) not in ('2','3','5') then cost else 0 end) cost3"
			",sum(case when substr(GRADE_ID,1,1) not in ('2','3','5') then cost else 0 end) cost4"
			" from tqmtscb01_mx"
			" where DATE_C = @stat_date"
			" )"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("prod_date", prod_date);
		cmd_inq.Parameters.Set("stat_date", prod_date.SubstringNE(0,6));
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_ret->Tables[1].Rows[0]["日产量"] = cmd_inq.GetDecimal(1);
			bcls_ret->Tables[1].Rows[0]["累计产量"] = cmd_inq.GetDecimal(2);
			if (cmd_inq.GetDecimal(1) != 0)
			{
				bcls_ret->Tables[1].Rows[0]["日成本"] = (cmd_inq.GetDecimal(3) / cmd_inq.GetDecimal(1)).Round(2);
			}
			if (cmd_inq.GetDecimal(2) != 0)
			{
				bcls_ret->Tables[1].Rows[0]["累计成本"] = (cmd_inq.GetDecimal(4) / cmd_inq.GetDecimal(2)).Round(2);
			}

			bcls_ret->Tables[1].Rows[0]["不锈钢日产量"] = cmd_inq.GetDecimal(5);
			bcls_ret->Tables[1].Rows[0]["不锈钢累计产量"] = cmd_inq.GetDecimal(6);
			if (cmd_inq.GetDecimal(5) != 0)
			{
				bcls_ret->Tables[1].Rows[0]["不锈钢日成本"] = (cmd_inq.GetDecimal(7) / cmd_inq.GetDecimal(5)).Round(2);
			}
			if (cmd_inq.GetDecimal(6) != 0)
			{
				bcls_ret->Tables[1].Rows[0]["不锈钢累计成本"] = (cmd_inq.GetDecimal(8) / cmd_inq.GetDecimal(6)).Round(2);
			}
		}
		cmd_inq.Close();

		sqlstr = " select sum(case when PROD_TIME like @prod_date||'%' and substr(st_no,1,1) in ('2','3','5') then CO2_WT else 0 end)"
			",sum(case when substr(st_no,1,1) in ('2','3','5') then CO2_WT else 0 end)"
			",sum(case when PROD_TIME like @prod_date || '%' and substr(st_no,1,1) not in ('2','3','5')   then CO2_WT else 0 end)"
			",sum(case when substr(st_no,1,1) not in ('2','3','5') then CO2_WT else 0 end)"
			",sum(case when PROD_TIME like  @prod_date||'%' then COST else 0 end)"
			",sum(COST)"
			" from ttksm02"
			" where PROD_TIME like @stat_date||'%'"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("prod_date", prod_date);
		cmd_inq.Parameters.Set("stat_date", prod_date.SubstringNE(0, 6));
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (bcls_ret->Tables[1].Rows[0]["日产量"].ToDecimal() != 0)
			{
				bcls_ret->Tables[1].Rows[0]["日碳排量"] = (cmd_inq.GetDecimal(1) / bcls_ret->Tables[1].Rows[0]["日产量"].ToDecimal()).Round(4);
			}
			if (bcls_ret->Tables[1].Rows[0]["累计产量"].ToDecimal() != 0)
			{
				bcls_ret->Tables[1].Rows[0]["累计碳排量"] = (cmd_inq.GetDecimal(2) / bcls_ret->Tables[1].Rows[0]["累计产量"].ToDecimal()).Round(4);
			}

			if (bcls_ret->Tables[1].Rows[0]["不锈钢日产量"].ToDecimal() != 0)
			{
				bcls_ret->Tables[1].Rows[0]["不锈钢日碳排量"] = (cmd_inq.GetDecimal(3) / bcls_ret->Tables[1].Rows[0]["不锈钢日产量"].ToDecimal()).Round(4);
			}
			if (bcls_ret->Tables[1].Rows[0]["不锈钢累计产量"].ToDecimal() != 0)
			{
				bcls_ret->Tables[1].Rows[0]["不锈钢累计碳排量"] = (cmd_inq.GetDecimal(4) / bcls_ret->Tables[1].Rows[0]["不锈钢累计产量"].ToDecimal()).Round(4);
			}

			bcls_ret->Tables[1].Rows[0]["日碳成本"] = (amount_tax*bcls_ret->Tables[1].Rows[0]["日碳排量"].ToDecimal()).Round(2);
			bcls_ret->Tables[1].Rows[0]["累计碳成本"] = (amount_tax*bcls_ret->Tables[1].Rows[0]["累计碳排量"].ToDecimal()).Round(2);
			bcls_ret->Tables[1].Rows[0]["不锈钢日碳成本"] = (amount_tax*bcls_ret->Tables[1].Rows[0]["不锈钢日碳排量"].ToDecimal()).Round(2);
			bcls_ret->Tables[1].Rows[0]["不锈钢累计碳成本"] = (amount_tax*bcls_ret->Tables[1].Rows[0]["不锈钢累计碳排量"].ToDecimal()).Round(2);


/*
			bcls_ret->Tables[1].Rows[0]["日碳成本"] = cmd_inq.GetDecimal(3);
			bcls_ret->Tables[1].Rows[0]["累计碳成本"] = cmd_inq.GetDecimal(4);*/
		}
		cmd_inq.Close();	

		

		sqlstr =
			" select st_no,PROD_WT,PROD_WT2,CO2_WT,CO2_WT2,CO2_WT_UNIT,CO2_WT_UNIT_AMT"
			",CO2_WT_CAL,CO2_WT_SUM_DES,CO2_WT_UNIT_CAL,CO2_WT_DIFF"
			",nvl(t2.GRADE_TYPE1,' ') GRADE_TYPE1,nvl(t2.GRADE_TYPE2,' ') GRADE_TYPE2,nvl(t2.GRADE_TYPE3,' ') GRADE_TYPE3,nvl(t2.GRADE_TYPE4,' ') GRADE_TYPE4,nvl(t2.GRADE_DESC,' ') GRADE_DESC"
			" from "
			" (select ST_NO,sum(prod_wt1) PROD_WT,sum(prod_wt2) PROD_WT2,sum(co2_wt1) CO2_WT,sum(co2_wt2) CO2_WT2"
			" ,case when sum(prod_wt1)!=0 then round(sum(co2_wt1)/sum(prod_wt1),6) else 0 end CO2_WT_UNIT"
			" ,case when sum(prod_wt1)!=0 then round(@amount_tax*sum(co2_wt1)/sum(prod_wt1),6) else 0 end CO2_WT_UNIT_AMT"			
			" ,sum(CO2_WT_CAL) CO2_WT_CAL,sum(co2_wt1)-sum(CO2_WT_CAL) CO2_WT_SUM_DES"
			" ,case when sum(prod_wt1)!=0 then round(sum(CO2_WT_CAL)/sum(prod_wt1),6) else 0 end CO2_WT_UNIT_CAL"
			" ,case when sum(prod_wt1)!=0 then round((sum(co2_wt1)-sum(CO2_WT_CAL))/sum(prod_wt1),6) else 0 end CO2_WT_DIFF"
			" from ("
			" select t.heat_no,t.st_no,sum(case when PROD_TIME like @prod_date||'%' then t.prod_wt else 0 end) prod_wt1"
			",sum(t.prod_wt) prod_wt2,sum(t.prod_wt*tt.CO2_WT) as CO2_WT_CAL"
			" from ttksm01 t left join ttk0005 tt on t.st_no=tt.ST_NO and t.BACKLOG_EA=tt.WHOLE_BACKLOG"
			" where 1=1"
			" and PROD_TIME like @prod_date || '%'"
			//" and stat_date = @stat_date"
			" group by t.heat_no,t.st_no ) t1"
			" left join "
			"( select heat_no,sum(co2_wt1) co2_wt1,sum(co2_wt2) co2_wt2"
			" from "
			" (select heat_no,sum(case when PROD_TIME like  @prod_date||'%' then co2_wt else 0 end) co2_wt1"
			",sum(co2_wt) co2_wt2"
			" from ttksm02"
			" where 1=1"
			" and PROD_TIME like @prod_date || '%'"
			//" and stat_date = @stat_date"
			" group by heat_no"
			" union all" //工序费
			" select heat_no, sum(t.prod_wt*tt.CO2_WT) co2_wt1"
			",sum(t.prod_wt*tt.CO2_WT) co2_wt2"
			" from ttksm01 t left join ttk0006 tt on t.BACKLOG_EA like '%'||tt.SUB_BACKLOG_CODE||'%'"
			" where 1=1"
			" and PROD_TIME like @prod_date || '%'"
			//" and stat_date = @stat_date"
			" group by heat_no"
			" union all" //工序费
			" select heat_no, sum(t.prod_wt*tt.CO2_WT) co2_wt1"
			",sum(t.prod_wt*tt.CO2_WT) co2_wt2"
			" from ttksm01 t left join ttk0006 tt on case when substr(t.st_no,1,1) in ('1','4') then '1C' else '2C' end = tt.SUB_BACKLOG_CODE"
			" where 1=1"
			" and PROD_TIME like @prod_date || '%'"
			//" and stat_date = @stat_date"
			" group by heat_no"
			" ) "
			" group by heat_no )t2 on t1.heat_no=t2.heat_no"			
			" group by st_no) t"
			" left join tQMTSB10 t2 on t.st_no=t2.STEEL_GRADE"
			" order by st_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("prod_date", prod_date);
		cmd_inq.Parameters.Set("stat_date", prod_date.SubstringNE(0, 6));
		cmd_inq.Parameters.Set("amount_tax", amount_tax);
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