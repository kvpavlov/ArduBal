const os = require("os");
// получим имя текущего пользователя
let userName = os.userInfo().username;
console.log(userName);

const greeting = require("../_1/greeting");
console.log(`Дата запроса: ${greeting.date}`);
console.log(greeting.getMessage(userName));

const User = require("./user.js");
let eugene = new User("PKV", 63);
eugene.sayHi();

var greeting1 = require("../_1/greeting.js");
greeting1.name = 'V1';
console.log(`Hello ${greeting1.name}`); //Hello Alice

var greeting2 = require("../_1/greeting.js");
greeting2.name= "Bob";

console.log(`Hello ${greeting2.name}`); //Hello Bob
// greeting1.name тоже изменилось
console.log(`Hello ${greeting1.name}`); //Hello Bob

const welcome = require("./welcome");

welcome.getMorningMessage();
welcome.getEveningMessage();