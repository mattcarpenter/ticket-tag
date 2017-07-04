const addon = require('bindings')('tickettag.node');
const fs = require('fs');

const image = fs.readFileSync('./board9.jpg');
const out = addon.extractFromImage(image);
console.log(out[0]);
fs.writeFileSync('./out.jpg', out[0].data, 'binary');
