select c.champion_name, count(champion_name) as cnt
from champ c, participant p
where c.champion_id = p.champion_id and p.position like "JUNGLE%"
group by c.champion_name
order by cnt desc
limit 3;
