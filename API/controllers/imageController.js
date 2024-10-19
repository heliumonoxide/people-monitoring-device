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