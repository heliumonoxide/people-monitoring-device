const express = require('express');
const router = express.Router();
const firebaseController = require('../controllers/firebaseController');

// POST route to handle image uploads
router.post('/upload', firebaseController.processImage);

// POST route to handle image uploads
router.get('/newest-image', firebaseController.getNewestImage);

// GET route to retrieve processed image results
router.get('/result', firebaseController.getNewestSum);

// GET route to retrieve highest count results
router.get('/highest', firebaseController.getHighestSum);

// GET route to retrieve highest count results
router.get('/ethic-count', firebaseController.getResultEthic);

// GET route to retrieve highest count results
router.get('/newest-sum', firebaseController.getNewestSumOnly);

// GET route to retrieve highest count results
router.get('/ethic-highest', firebaseController.getResultEthicHighest);

module.exports = router;
