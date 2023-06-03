lix = require('lix')

lix.database = {
    hostname = "localhost",
    port     = 5003,
    username = "admin",
    password = "password"
}

lix.cache = {
    client = "redis",
    port = 6000
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

Tx = require("sql.Tx")
function Tx:Event(topic, data, meta)
   self.query("INSERT INTO event.event", topic) 
end

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
        name = user.Name,
        age = toAge(user.DOB)
    }
end

function payment(req, res)
   tx = lix.begin_tx()
  
    result = tx.query_file("payment.sql", req.payment_id)

    stripe.customer.create({
        name = result.first_name
    })

    tx.Event("payment.created", result)
   tx.commit()
end
