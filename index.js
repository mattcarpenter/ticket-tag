const addon = require('bindings')('factorial.node');

console.log(addon.factorial(10)); // 3628800