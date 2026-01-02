
import sys
import os
import json
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, 
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                    filename='image_translator.log')
logger = logging.getLogger("ImageTranslator")

class ImageTranslator:
    def __init__(self):
        self.reader = None
        self.use_gpu = True
        self.available = False
        
        try:
            import easyocr
            import torch
            self.use_gpu = torch.cuda.is_available()
            logger.info(f"EasyOCR initialized. GPU Available: {self.use_gpu}")
            self.easyocr = easyocr
            self.available = True
        except ImportError:
            logger.warning("EasyOCR or Torch not found. Running in MOCK mode.")
            self.available = False
        except Exception as e:
            logger.error(f"Failed to initialize EasyOCR: {e}")
            self.available = False

    def is_available(self):
        return self.available

    def translate_image(self, image_path, source_lang='en', target_lang='th'):
        """
        Translates text in an image.
        Returns a JSON string containing list of detections:
        [
            {
                "text": "Detected Text",
                "bbox": [[x1, y1], [x2, y2], [x3, y3], [x4, y4]],
                "confidence": 0.98
            },
            ...
        ]
        """
        results = []
        
        if not os.path.exists(image_path):
            logger.error(f"Image not found: {image_path}")
            return json.dumps({"error": "Image file not found"})

        if self.available:
            try:
                # Initialize reader for the specific language if not already cached/loaded
                # EasyOCR reader efficiently handles reloading usually, but we can optimize if needed.
                # For now, instantiate/use reader on demand or keep a persistent one if languages match.
                # Simplest for this Integration: Create reader for requested langs.
                # Note: 'en' is usually included by default or as a mix.
                
                langs = [source_lang]
                if 'en' not in langs: langs.append('en') # Always good to have english fallback
                
                reader = self.easyocr.Reader(langs, gpu=self.use_gpu)
                
                # detail=1 returns (bbox, text, prob)
                detections = reader.readtext(image_path, detail=1)
                
                for bbox, text, prob in detections:
                    # bbox is list of 4 points [[x,y], [x,y], ...], need to convert to list of lists for JSON
                    bbox_list = [ [int(p[0]), int(p[1])] for p in bbox ]
                    
                    results.append({
                        "text": text,
                        "bbox": bbox_list,
                        "confidence": float(prob)
                    })
                    
            except Exception as e:
                logger.error(f"Error during OCR: {e}")
                return json.dumps({"error": str(e)})
        else:
            # MOCK MODE
            logger.info("Mocking OCR result")
            results = [
                {
                    "text": "Hello World",
                    "bbox": [[50, 50], [200, 50], [200, 100], [50, 100]],
                    "confidence": 0.99
                },
                {
                    "text": "Sample Text",
                    "bbox": [[50, 150], [200, 150], [200, 200], [50, 200]],
                    "confidence": 0.95
                }
            ]
            
        return json.dumps(results, ensure_ascii=False)

# Global instance
translator_instance = ImageTranslator()
