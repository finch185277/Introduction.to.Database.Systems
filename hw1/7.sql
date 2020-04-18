select 'DUO_CARRY' as position, c.champion_name, p2.kda
from (
  select p1.champion_id, (p1.k + p1.a) / p1.d as kda
  from (
    select p.champion_id, sum(s.kills) as k, sum(s.deaths) as d, sum(s.assists) as a
    from participant p, stat s
    where p.position like 'DUO_CARRY%' and p.player_id = s.player_id
    group by p.champion_id
  ) as p1
  where p1.d != 0
  order by kda desc
  limit 1
) as p2, champ c
where c.champion_id = p2.champion_id
union
select 'DUO_SUPPORT' as position, c.champion_name, p2.kda
from (
  select p1.champion_id, (p1.k + p1.a) / p1.d as kda
  from (
    select p.champion_id, sum(s.kills) as k, sum(s.deaths) as d, sum(s.assists) as a
    from participant p, stat s
    where p.position like 'DUO_SUPPORT%' and p.player_id = s.player_id
    group by p.champion_id
  ) as p1
  where p1.d != 0
  order by kda desc
  limit 1
) as p2, champ c
where c.champion_id = p2.champion_id
union
select 'JUNGLE' as position, c.champion_name, p2.kda
from (
  select p1.champion_id, (p1.k + p1.a) / p1.d as kda
  from (
    select p.champion_id, sum(s.kills) as k, sum(s.deaths) as d, sum(s.assists) as a
    from participant p, stat s
    where p.position like 'JUNGLE%' and p.player_id = s.player_id
    group by p.champion_id
  ) as p1
  where p1.d != 0
  order by kda desc
  limit 1
) as p2, champ c
where c.champion_id = p2.champion_id
union
select 'MID' as position, c.champion_name, p2.kda
from (
  select p1.champion_id, (p1.k + p1.a) / p1.d as kda
  from (
    select p.champion_id, sum(s.kills) as k, sum(s.deaths) as d, sum(s.assists) as a
    from participant p, stat s
    where p.position like 'MID%' and p.player_id = s.player_id
    group by p.champion_id
  ) as p1
  where p1.d != 0
  order by kda desc
  limit 1
) as p2, champ c
where c.champion_id = p2.champion_id
union
select 'TOP' as position, c.champion_name, p2.kda
from (
  select p1.champion_id, (p1.k + p1.a) / p1.d as kda
  from (
    select p.champion_id, sum(s.kills) as k, sum(s.deaths) as d, sum(s.assists) as a
    from participant p, stat s
    where p.position like 'TOP%' and p.player_id = s.player_id
    group by p.champion_id
  ) as p1
  where p1.d != 0
  order by kda desc
  limit 1
) as p2, champ c
where c.champion_id = p2.champion_id;
