
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

    def inpaint_text_regions(self, image_path, detections_json):
        """
        Remove text from image using AI (LaMa) inpainting and detect background colors.
        
        Returns:
            JSON string containing enriched detections with angle and text color.
        """
        try:
            import cv2
            import numpy as np
            import math
            from simple_lama_inpainting import SimpleLama
            from PIL import Image
        except ImportError as e:
            logger.error(f"Required library not available: {e}. Please run 'pip install simple-lama-inpainting'")
            # Fallback imports for color detection only if LaMa fails
            try:
                import cv2
                import numpy as np
                import math
            except:
                return json.dumps({"error": "OpenCV not available"})
        
        # Load image with OpenCV for processing
        img = cv2.imread(image_path)
        if img is None:
            return json.dumps({"error": "Failed to load image"})
            
        detections = json.loads(detections_json)
        mask = np.zeros(img.shape[:2], dtype=np.uint8)
        enriched_detections = []

        for detection in detections:
            bbox = detection.get("bbox", [])
            if len(bbox) >= 4:
                pts = np.array(bbox, dtype=np.int32)
                cv2.fillPoly(mask, [pts], 255)
                
                # --- Advanced Analysis (Same as before) ---
                x_min = max(0, min(p[0] for p in bbox))
                y_min = max(0, min(p[1] for p in bbox))
                x_max = min(img.shape[1], max(p[0] for p in bbox))
                y_max = min(img.shape[0], max(p[1] for p in bbox))
                
                roi = img[y_min:y_max, x_min:x_max]
                
                # Defaults
                bg_color = [255, 255, 255]
                text_color = [0, 0, 0]
                angle = 0.0
                is_dark = False

                if roi.size > 0:
                    # 1. Calculate Rotation Angle (from top edge)
                    p0 = bbox[0]
                    p1 = bbox[1]
                    dx = p1[0] - p0[0]
                    dy = p1[1] - p0[1]
                    angle = math.degrees(math.atan2(dy, dx))
                    
                    # 2. Extract Colors (Text vs BG) using Otsu binarization
                    try:
                        gray_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
                        _, bin_img = cv2.threshold(gray_roi, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
                        
                        white_pixels = cv2.countNonZero(bin_img)
                        total_pixels = bin_img.size
                        
                        if white_pixels > total_pixels / 2:
                            mask_text = cv2.bitwise_not(bin_img)
                            mask_bg = bin_img
                        else:
                            mask_text = bin_img
                            mask_bg = cv2.bitwise_not(bin_img)
                            
                        if cv2.countNonZero(mask_text) > 0:
                            text_mean = cv2.mean(roi, mask=mask_text)[:3]
                            text_color = [int(text_mean[2]), int(text_mean[1]), int(text_mean[0])]
                        
                        if cv2.countNonZero(mask_bg) > 0:
                            bg_mean = cv2.mean(roi, mask=mask_bg)[:3]
                            bg_color = [int(bg_mean[2]), int(bg_mean[1]), int(bg_mean[0])]
                    except Exception as e:
                        avg = cv2.mean(roi)[:3]
                        bg_color = [int(avg[2]), int(avg[1]), int(avg[0])]
                
                luminance = (0.299 * bg_color[0] + 0.587 * bg_color[1] + 0.114 * bg_color[2]) / 255.0
                is_dark = luminance < 0.5
                
                detection["bg_color"] = bg_color
                detection["text_color"] = text_color
                detection["angle"] = angle
                detection["is_dark"] = is_dark
                
                enriched_detections.append(detection)
        
        # --- LaMa Inpainting ---
        try:
            # Dilate mask for better coverage
            kernel = np.ones((5,5), np.uint8)
            mask_dilated = cv2.dilate(mask, kernel, iterations=3)
            
            # Convert to PIL for LaMa
            img_pil = Image.fromarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
            mask_pil = Image.fromarray(mask_dilated)
            
            lama = SimpleLama()
            result_pil = lama(img_pil, mask_pil)
            
            # Save output
            import tempfile
            output_path = os.path.join(tempfile.gettempdir(), f"lama_inpainted_{os.path.basename(image_path)}")
            result_pil.save(output_path)
            
            logger.info(f"LaMa Inpainting success: {output_path}")
            
        except Exception as e:
            logger.error(f"LaMa Inpainting failed: {e}. Falling back to OpenCV.")
            # Fallback to OpenCV Telea
            kernel = np.ones((3,3), np.uint8)
            mask_dilated = cv2.dilate(mask, kernel, iterations=3)
            inpainted = cv2.inpaint(img, mask_dilated, 5, cv2.INPAINT_TELEA)
            
            import tempfile
            output_path = os.path.join(tempfile.gettempdir(), f"cv2_inpainted_{os.path.basename(image_path)}")
            cv2.imwrite(output_path, inpainted)
        
        return json.dumps({
            "inpainted_path": output_path,
            "detections": enriched_detections
        })

# Global instance
translator_instance = ImageTranslator()
