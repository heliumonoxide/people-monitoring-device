const { db, bucket } = require('../configs/firebase'); // Import your firebase setup
// Process image upload
exports.processImage = (req, res) => {
    // Logic to process the image, e.g., save to Firebase or other cloud storage
    const { imageUrl } = req.body;

    // Assuming some processing happens here
    console.log('Received image URL:', imageUrl);

    // Send a valid JSON response
    res.json({ message: 'Image uploaded successfully!', imageUrl });
};

// Get image processing result
exports.getNewestSum = async (req, res) => {
    try {
        const snapshot = await db
            .collection('persons-sum')
            .orderBy('timeAdded', 'desc') // Order by timeAdded field in descending order
            .limit(10) // Limit the results to 1 document
            .get();

        // Check if any documents were returned
        if (snapshot.empty) {
            return res.status(404).send('No data found.');
        }

        const numbers = [];
        snapshot.forEach(doc => {
            numbers.push({ id: doc.id, sum: doc.data().sum, timeAdded: doc.data().timeAdded });
        });

        res.status(200).json(numbers); // Return the newest document as a single object
    } catch (error) {
        console.error('Error reading from Firestore:', error);
        res.status(500).send('Error reading from Firestore.');
    }
};

// Get image processing result
exports.getNewestSumOnly = async (req, res) => {
    try {
        const snapshot = await db
            .collection('persons-sum')
            .orderBy('timeAdded', 'desc') // Order by timeAdded field in descending order
            .limit(1) // Limit the results to 1 document
            .get();

        // Check if any documents were returned
        if (snapshot.empty) {
            return res.status(404).send('No data found.');
        }

        const numbers = [];
        snapshot.forEach(doc => {
            numbers.push({ id: doc.id, sum: doc.data().sum, timeAdded: doc.data().timeAdded });
        });

        res.status(200).json(numbers); // Return the newest document as a single object
    } catch (error) {
        console.error('Error reading from Firestore:', error);
        res.status(500).send('Error reading from Firestore.');
    }
};

// Get images from firebase
exports.getNewestImage = async (req, res) => {
    try {
        const [files] = await bucket.getFiles({ prefix: 'results/' });  // Adjust the prefix to your folder
        if (files.length === 0) {
            return res.status(404).json({ message: 'No images found' });
        }

        // Sort files by timeCreated (newest first)
        const sortedFiles = files.sort((a, b) => new Date(b.metadata.timeCreated) - new Date(a.metadata.timeCreated));
        const newestFile = sortedFiles[0];

        // Get signed URL for the newest image (valid for 1 hour, adjust as needed)
        const [url] = await newestFile.getSignedUrl({
            action: 'read',
            expires: '03-17-2025',  // Set the expiration date for the signed URL
        });

        const [metadata] = await newestFile.getMetadata();

        res.json({ imageUrl: url, metaData: metadata });
    } catch (error) {
        console.error('Error retrieving image:', error);
        res.status(500).json({ message: 'Error retrieving image' });
    }
};

exports.getHighestSum = async (req, res) => {
    try {
        const snapshot = await db
            .collection('persons-sum')
            .orderBy('sum', 'desc')
            .limit(1) // Limit the results to 1 document
            .get();

        // Check if any documents were returned
        if (snapshot.empty) {
            return res.status(404).send('No data found.');
        }

        const numbers = [];
        snapshot.forEach(doc => {
            numbers.push({ id: doc.id, sum: doc.data().sum, timeAdded: doc.data().timeAdded });
        });

        res.status(200).json(numbers); // Return the newest document as a single object
    } catch (error) {
        console.error('Error reading from Firestore:', error);
        res.status(500).send('Error reading from Firestore.');
    }
};

exports.getResultEthic = async (req, res) => {
    try {
        const snapshot = await db
            .collection('ethical-count')
            .orderBy('timeAdded', 'desc')
            .limit(10) // Limit the results to 1 document
            .get();

        // Check if any documents were returned
        if (snapshot.empty) {
            return res.status(404).send('No data found.');
        }

        const numbers = [];
        snapshot.forEach(doc => {
            numbers.push({ id: doc.id, speed: doc.data().speed, timeAdded: doc.data().timeAdded });
        });

        res.status(200).json(numbers); // Return the newest document as a single object
    } catch (error) {
        console.error('Error reading from Firestore:', error);
        res.status(500).send('Error reading from Firestore.');
    }
};

exports.getResultEthicHighest = async (req, res) => {
    try {
        const snapshot = await db
            .collection('ethical-count')
            .orderBy('speed', 'desc')
            .limit(1) // Limit the results to 1 document
            .get();

        // Check if any documents were returned
        if (snapshot.empty) {
            return res.status(404).send('No data found.');
        }

        const numbers = [];
        snapshot.forEach(doc => {
            numbers.push({ id: doc.id, speed: doc.data().speed, timeAdded: doc.data().timeAdded });
        });

        res.status(200).json(numbers); // Return the newest document as a single object
    } catch (error) {
        console.error('Error reading from Firestore:', error);
        res.status(500).send('Error reading from Firestore.');
    }
};