const TicketTag = require('bindings')('tickettag.node');
const fs = require('fs');

var characterMap;

module.exports = {
  extractFromImage: extractFromImage,
  create: create
};

function extractFromImage(imageData) {
  return TicketTag.extractFromImage(imageData);
}

function create(text) {
  if (!characterMap) {
    characterMap = fs.readFileSync('./data/character-map.png');
  }

  return TicketTag.create(text, characterMap);
}