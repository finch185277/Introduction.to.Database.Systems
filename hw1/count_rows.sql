SELECT
    TABLE_NAME,
    TABLE_ROWS
FROM
    `information_schema`.`tables`
WHERE
    `table_schema` = 'hw1';
