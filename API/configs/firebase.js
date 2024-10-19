const admin = require('firebase-admin');
const serviceAccount = require('../env/despro-project-monitoring-firebase-adminsdk-ze39m-e0a5a52f3f.json'); // Update this path

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  storageBucket: 'gs://despro-project-monitoring.appspot.com', // Replace with your storage bucket
});

const db = admin.firestore();
const bucket = admin.storage().bucket();

module.exports = { db, bucket };