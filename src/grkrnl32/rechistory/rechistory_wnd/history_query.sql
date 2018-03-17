execute block (lst blob = :lst,beg_tm bigint = :beg_tm, end_tm bigint = :end_tm,archive integer = :archive)
returns
(
REC_ID INTEGER,
DIAG SMALLINT,
STATE SMALLINT,
RVALUE BIGINT,
CH_TIME BIGINT,
is_archive integer
)
as
begin
 diag  = 0;
 state = NULL;
 rvalue  = 0;
 ch_time = 0;
 for
  select l.id from usp_list_to_rows(:lst) l into :rec_id
  do
     suspend;

for
 select
  r.rec_id
 ,r.diag
 ,r.state
 ,r.rvalue
 ,r.ch_time
 ,r.archive
   from rechistory r
   join usp_list_to_rows(:lst) l on l.id = r.rec_id and  r.ch_time between :beg_tm and :end_tm
   --and (coalesce(r.archive,0) <> 0 or 0=(coalesce(:archive,0))) -- если выбрано только архивные данные
 order by r.ch_time
 into :rec_id,:diag,:state,:rvalue,:ch_time,:is_archive
 do
  begin
   suspend;
  end
end

