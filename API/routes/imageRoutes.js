const express = require('express');
const router = express.Router();
const imageController = require('../controllers/imageController');

// POST route to handle image uploads
router.post('/upload', imageController.processImage);

// GET route to retrieve processed image results
router.get('/result', imageController.getNewestSum);

module.exports = router;
