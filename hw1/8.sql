select c.champion_name
from champ c
where c.champion_id not in (
  select distinct t.champion_id
  from match_info m, teamban t
  where m.match_id = t.match_id and SUBSTRING_INDEX(m.version, '.', 2) like '7.7'
)
order by c.champion_name;
