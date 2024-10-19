// eslint-disable-next-line no-unused-vars
import React, { useState } from 'react';

const ImageUploader = () => {
  const [imageUrl, setImageUrl] = useState('');

  // The function to handle image upload
  const uploadImage = async (imageUrl) => {
    try {
      const response = await fetch('/api/upload', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ imageUrl }),
      });

      const data = await response.json();
      console.log('Image uploaded successfully:', data);
    } catch (error) {
      console.error('Error uploading image:', error);
    }
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    if (imageUrl) {
      uploadImage(imageUrl);  // Call the function on form submit or button click
    }
  };

  return (
    <div>
      <h2>Upload an Image</h2>
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          value={imageUrl}
          onChange={(e) => setImageUrl(e.target.value)}
          placeholder="Enter image URL"
        />
        <button type="submit">Upload Image</button>
      </form>
    </div>
  );
};

export default ImageUploader;
