select 'DUO_CARRY' as position, c.champion_name
from (
  select p.champion_id, count(*) as cnt
  from (
    select m.match_id
    from match_info m
    where m.duration between 40 * 60 and 50 * 60
  ) as m1, participant p
  where p.match_id = m1.match_id and p.position like 'DUO_CARRY%'
  group by p.champion_id
  order by cnt desc
  limit 1
) as cid, champ c
where c.champion_id = cid.champion_id
union
select 'DUO_SUPPORT' as position, c.champion_name
from (
  select p.champion_id, count(*) as cnt
  from (
    select m.match_id
    from match_info m
    where m.duration between 40 * 60 and 50 * 60
  ) as m1, participant p
  where p.match_id = m1.match_id and p.position like 'DUO_SUPPORT%'
  group by p.champion_id
  order by cnt desc
  limit 1
) as cid, champ c
where c.champion_id = cid.champion_id
union
select 'JUNGLE' as position, c.champion_name
from (
  select p.champion_id, count(*) as cnt
  from (
    select m.match_id
    from match_info m
    where m.duration between 40 * 60 and 50 * 60
  ) as m1, participant p
  where p.match_id = m1.match_id and p.position like 'JUNGLE%'
  group by p.champion_id
  order by cnt desc
  limit 1
) as cid, champ c
where c.champion_id = cid.champion_id
union
select 'MID' as position, c.champion_name
from (
  select p.champion_id, count(*) as cnt
  from (
    select m.match_id
    from match_info m
    where m.duration between 40 * 60 and 50 * 60
  ) as m1, participant p
  where p.match_id = m1.match_id and p.position like 'MID%'
  group by p.champion_id
  order by cnt desc
  limit 1
) as cid, champ c
where c.champion_id = cid.champion_id
union
select 'TOP' as position, c.champion_name
from (
  select p.champion_id, count(*) as cnt
  from (
    select m.match_id
    from match_info m
    where m.duration between 40 * 60 and 50 * 60
  ) as m1, participant p
  where p.match_id = m1.match_id and p.position like 'TOP%'
  group by p.champion_id
  order by cnt desc
  limit 1
) as cid, champ c
where c.champion_id = cid.champion_id;
