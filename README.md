

# Machbase Info

To write queries:
Topic: db/query

To create table:
{
    "q": "create tag table if not exists moisture (id varchar(50) primary key, time datetime basetime, value double summarized, device_id varchar(20));",
    "format": "csv",
    "reply": "db/reply/my_query"
}
{
    "q": "SELECT * FROM moisture;",
    "format": "csv",
    "reply": "db/reply/my_query"
}

to write data (I think). basically a list of lists or just a single list
```
```plaintext
[["id_value", 1, 1.04, "device_id_1"], ["id_value2", 2, 9.44, "device_id_1"]]
<!-- or -->
["id_value2", 2, 9.44, "device_id_1"]
```
