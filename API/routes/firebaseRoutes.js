const express = require('express');
const router = express.Router();
const firebaseController = require('../controllers/firebaseController');

// POST route to handle image uploads
router.post('/upload', firebaseController.processImage);

// POST route to handle image uploads
router.get('/newest-image', firebaseController.getNewestImage);

// GET route to retrieve processed image results
router.get('/result', firebaseController.getNewestSum);

module.exports = router;
