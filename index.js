const ticketTag = require('./lib/ticket-tag');
const fs = require('fs');

module.exports = {
  extractFromImage: ticketTag.extractFromImage,
  create: ticketTag.create
};

