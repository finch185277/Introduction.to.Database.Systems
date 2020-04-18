-- select match_id, concat(convert(floor(duration / 60 / 60), char), ':', convert((floor(duration / 60)) % 60, char), ':', convert(duration % 60, char)) as time
select match_id, SEC_TO_TIME(duration) as time
from match_info
order by duration desc
limit 5;
