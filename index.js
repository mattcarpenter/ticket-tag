const ticketTag = require('./lib/ticket-tag');
const fs = require('fs');

const image = fs.readFileSync('./board9.jpg');
const out = ticketTag.extractFromImage(image);
fs.writeFileSync('./out.jpg', out[0].data, 'binary');

//var img = ticketTag.create("DIDFE-1922");
//console.log(img);
//fs.writeFileSync('./tag.jpg', img, 'binary');
