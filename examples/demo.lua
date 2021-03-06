
lix.database = {
    hostname = "localhost",
    port     = 5003,
    username = "admin",
    password = "password"
}

lix.http.defaultContentType = "application/json"

lix.routes = {
    {"POST", "/", home},
    {"GET", "/", home},
}

local homesql = [[
    SELECT *
    FROM user
    WHERE user_id = $1
]]

function home(req, body, res)
    userId = req.var["user_id"]
    ret = lix.sqlquery(homesql, userId)

    lix.get("http://www.google.com")
    lix.post("http://www.google.com", nil, data{})

    res.header({"Content-Type", "application/json"})
    res.status(200)
    return ret
end

function users(req, body, res)
    userId = req.var["user_id"]
    user = lix.sqlquery(homesql, userId)
    return {
        name = user.Name
        age = toAge(user.DOB)
    }
end

