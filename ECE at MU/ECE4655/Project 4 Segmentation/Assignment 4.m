% thresholding 
% show each intermediate threshold

% k means
% show each clustering pass
%%%%%

% Assignment 4 Thresholding, k-means clustering, and connected component analysis

% filename = 'C:\Users\PhotonUser\Desktop\P4img\1001a';
% filename = 'C:\Users\PhotonUser\Desktop\P4img\1001d';
% filename = 'C:\Users\PhotonUser\Desktop\P4img\1034a';
% filename = 'C:\Users\PhotonUser\Desktop\P4img\1026a';
 filename = 'C:\Users\PhotonUser\Desktop\P4img\1027a';
% filename = 'C:\Users\PhotonUser\Desktop\P4img\1043a';

% load test image and convert to double
 img = imread(strcat(filename, '.tif'));

dimg = double(img);
timg = dimg;
% grayscale image for thresholding if input is color image
% timg = double(rgb2gray(img));


%%%%%%%%%%%%%%%%%%
% Thresholding
%%%%%%%%%%%%%%%%%%

% variable to hold the thresholded image
fimg = img;

% set initial threshold value and minimum difference
% initial threshold set to average gray value of image
threshold = mean(mean(timg));
threshmin = 1;

% loop to do thresholding and update the threshold value
% each iteration. breaks when current threshold and updated threshold
% are below a minimum difference
T = true;
iteration = 1;
while T
	low = find(timg <= threshold);
	hi = find(timg > threshold);

	%generate and display thresholded image
	fimg(low) = 0;
	fimg(hi) = 255;
	imshow(uint8(fimg));
	
	% write intermediate images to new files
	imwrite(uint8(fimg), strcat(filename, ' int', string(iteration), '.tif'));
	iteration = iteration + 1;
	
	z = waitforbuttonpress;

	meanlow = mean(timg(low));
	meanhi = mean(timg(hi));
	
	if abs(threshold - (meanlow + meanhi)/2) < threshmin
		T = false;
	else
		threshold = (meanlow + meanhi)/2;
	end
end

%fimg is the final threshold result after loop ends.
imwrite(uint8(fimg), strcat(filename, ' final', '.tif'));

%%%%%%%%%%%%%%%%%%%%%%%
% K-Means Clustering
%%%%%%%%%%%%%%%%%%%%%%%

% This set proceeds using the same image as the thresholding
% the original image (img) is unaltered, and the grayscale version (timg) will be the same as well.


%set K value for number of clusters and make array of k-values size K
K = 16;
k = zeros(K,1);

% uniform distribution of k-values
kdiff = 255/K;
for i = 1:K
	k(i) = kdiff*i - (kdiff/2);
end

% matrix of K rows that holds indices of each pixel assigned to cluster k
% dimension 2 initialized to total pixels / 8
k_clusters = zeros(K, 290000);

% index for last valid element in each row of k-clusters. 
% needed since k-clusters will be width = largest cluster, and smaller rows will have zero padding at the ends
cluster_ends = zeros(K,1);

% maximum iterations for the k-means optimization
max_iterations = 1000;


% K-Means Loop, iterates until the points are not changing, or it reaches max_iterations
for i = 1:max_iterations
	
	% variable to store the original clusters for comparison after each iteration
	if i > 1
		k_current = k_clusters;
	end

	% loop to fill the k-clusters array and set the cluster_ends values
	for j = 1:K
		if j == 1
			x = find(timg < k(j) + kdiff);
		elseif j < K
			x = find(timg >= k(j) - kdiff & timg < k(j) + kdiff);
		else
			x = find(timg >= k(j) - kdiff);
		end
		
		x = transpose(x);
		cluster_ends(j) = length(x);
		k_clusters(j,1:cluster_ends(j)) = x;

	end
	
	% update the cluster centers
	for n = 1:K
		if cluster_ends(n) > 0
			k(n) = mean(timg(k_clusters(n,1:cluster_ends(n))));
		end
	end
	
	% display intermediate images every 10 iterations
	if mod(i, 5) == 0
		for m = 1:K
			fimg(k_clusters(m,1:cluster_ends(m))) = k(m);
		end
		
		imwrite(uint8(fimg), strcat(filename, 'k-means int', string(i), '.tif'));
		imshow(uint8(fimg));
		z = waitforbuttonpress;
	end
	
	% check if the clusters changed, and if the did not, exit loop
	if  i > 1 & k_clusters == k_current
		break;
	end
end

% remove duplicate clusters after convergence
T = true;
i = 1;
while T
	if cluster_ends(i) == 0
		k_clusters(i,:) = [];
		cluster_ends(i) = [];
		k(i) = [];
		K = K - 1;
		i = i - 1;
	elseif i < K
		if cluster_ends(i) == cluster_ends(i+1) & k_clusters(i, 1:cluster_ends(i)) == k_clusters(i + 1, 1:cluster_ends(i + 1))
			k_clusters(i,:) = [];
			cluster_ends(i) = [];
			k(i) = [];
			K = K - 1;
			i = i - 1;
		end
	end
	i = i + 1k
		
	if i > K
		break;
	end
end

% update pixels after final iteration
for i = 1:K
	fimg(k_clusters(i,1:cluster_ends(i))) = k(i);
end

imshow(uint8(fimg));
imwrite(uint8(fimg), strcat(filename, ' k-means final.tif'));

% connected component analysis one iteration per cluster
for i = 1:K

	%make a separate image holding 1 cluster and convert to binary
	cluster = zeros(size(fimg));
	cluster(k_clusters(i,1:cluster_ends(i))) = 1;
	cluster = im2bw(cluster,.9);
	
	imwrite(cluster, strcat(filename, ' cluster ', string(i), '.tif'));
	imshow(cluster);
	z = waitforbuttonpress;
	
	%get connected components
	CC = bwconncomp(cluster);
	
	%get centroids of the components
	centroids = regionprops(CC, 'Centroid');
end









