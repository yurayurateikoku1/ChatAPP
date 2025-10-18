const config_module = require("./config")
const Redis = require("ioredis");

const RedisCli = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_passwd
});

RedisCli.on('error', (err) => {
    console.log('Redis Client Error', err)
    RedisCli.quit();
});

async function GetRedis(key) {
    try {
        const result = await RedisCli.get(key);
        if (result == null) {
            console.log("result,<" + result + " >is cannot find");
            return null;
        }
        console.log("result,<" + result + " >is found");
        return result;
    } catch (error) {
        console.error('Getting Redis is:', error);
        return null;
    }
}

async function QueryRedis(key) {
    try {
        const result = await RedisCli.exists(key);
        if (result == 0) {
            console.log("result,<" + result + " >is null");
            return null;
        }
        console.log("result,<" + result + " >is found");
        return result;
    } catch (error) {
        console.error('Querying Redis is:', error);
        return null;
    }
}

async function SetRedisExpire(key, value, exptime) {
    try {
        await RedisCli.set(key, value,)
        await RedisCli.expire(key, exptime);
        return true;
    } catch (error) {
        console.error('Setting Redis is:', error);
        return false;
    }
}

function Quit() {
    RedisCli.quit();
}

module.exports = { GetRedis, SetRedisExpire, QueryRedis, Quit };