import cv2 

class CVMapping():
    def RGB2Binary(self, state_img):
        
        h = int(state_img.shape[0]*0.5)
        w = 80

        _grey = cv2.cvtColor(state_img, cv2.COLOR_BGR2GRAY)

        _, _bin = cv2.threshold(_grey, 5, 255, cv2.THRESH_BINARY)
        bin_img = cv2.resize(_bin, (w,h), interpolation=cv2.INTER_AREA)

        bin_img = bin_img[25:, :]

        bin_img = bin_img.reshape((80,80))##input network size

        return bin_img

        
