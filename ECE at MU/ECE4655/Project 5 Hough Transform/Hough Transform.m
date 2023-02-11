% Assignment 5, Hough Transform
% This program takes in an image, converts to binary using canny edge detection
% then finds the hough transform in polar coordinates.

% load the image to transform
% img = imread('C:\Users\PhotonUser\Desktop\building.jpg');
 img = imread('C:\Users\PhotonUser\Desktop\campus.jpg');
% img = imread('C:\Users\PhotonUser\Desktop\castle.jpg');
% img = imread('C:\Users\PhotonUser\Desktop\road.jpg');
% img = imread('C:\Users\PhotonUser\Desktop\rock.jpg');
% img = imread('C:\Users\PhotonUser\Desktop\roof-top.jpg');

imsize = size(img);

% initialize the maximum values for the polar plot
% theta from -90 to 90 degrees, rho from 0 to diagonal length of image
thetas = -90:1:90;
rho_size = int16(sqrt(imsize(1)^2 + imsize(2)^2));
rhos = -rho_size:1:rho_size;

% initialize the accumulator matrix for storing the polar values
A = zeros(length(rhos),length(thetas));

% create arrays holding the cosine,sine values for each theta values
cos_thetas = cosd(thetas);
sin_thetas = sind(thetas);

% create binary edgemap using built-in edge function
BWimg = edge(rgb2gray(img), 'Canny');

% find array of indices for edge pixels in the edgemap and convert to integer x, y coords
I = find(BWimg);
[y,x] = ind2sub(imsize, I);
[theta,rho] = cart2pol(x,y);
rho = int16(rho);

% change x,y to be in range -d to d
x = int16(x) ;%- int16(rho_size/2);
y = int16(y) ;%- int16(rho_size/2);

% create 2 arrays holding the x,y coordinates and the sin,cos values
% multiply them to get all rho values for each edge pixel
c_coords = [transpose(x);transpose(y)];
trig_vals = [cos_thetas;sin_thetas];

% matrix with every rho value for every theta value 
% only for the edge pixels
rho_vals = double(transpose(c_coords)) * trig_vals;

rho_vals = int16(rho_vals);

% loop through the array of rho values and increment the accumulator
% where the rho value is the row and theta is the column in the accumulator
for i = 1:size(thetas,2)
	for j = 1:size(rho_vals, 1);
		u = find(rhos == rho_vals(j,i));
		A(u, i) = A(u, i) + 1;
	end
end

% A = flip(A,1);
% A = flip(A,2);

% copy the original image
finalimg = img;

% threshold the Hough transform to get the maxima
[rh,th] = find(A > .9 * max(max(A)));

x = zeros(2,1);
y = zeros(2,1);

subplot(1,4,1);
imshow(img)
subplot(1,4,2);
imshow(BWimg)
subplot(1,4,3);
imshow(uint8(A))
hold on
plot(th,rh,'+','color','red')
subplot(1,4,4);
imshow(finalimg), hold on
%using the maxima, plot their lines on the copied image
for i = 1:length(rh)
	x(1) = (rh(i)*cosd(th(i)) + rho_size);
	y(1) = (rh(i)*sind(th(i)) + rho_size);
	x(2) = (x(1) + 1000*cosd(th(i)));
	y(2) = (y(1) + 1000*sind(th(i)));
	x(1) = x(1) - 1000*cosd(th(i));
	y(1) = y(1) - 1000*sind(th(i));
	line(x,y);
end










