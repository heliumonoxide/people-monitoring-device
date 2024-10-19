const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');

const imageRoutes = require('./routes/imageRoutes');
const { db, bucket } = require('./configs/firebase');

const app = express();
const port = process.env.PORT || 5000;

app.use(cors());
app.use(bodyParser.json());

// Use the imported routes
app.use('/api', imageRoutes);

// Example root endpoint
app.get('/', (req, res) => {
  res.send('Hello from the Express server!');
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});