# Cache-Content-Delivery-Network

CacheCDN is a content delivery network (CDN) simulator, implementing caching mechanisms and freshness count. It efficiently stores and retrieves both text and binary files.

Features
Caching: Stores text and binary files in memory for quick access.
Freshness Count: Each file in cache has a freshness count which is decremented on every access until it reaches zero. When the count reaches zero, the file is reloaded from the disk.
Top File: It tracks the most frequently accessed file.
Cache Statistics: Provides statistics like total requests, total files requested, average requests per file, the most requested file, and total disk reads.
How to Use
Please refer to the class Cache in cacheCDN.h and its methods for usage.

Please note, this is a simulator and is not intended to connect to real network resources or serve actual files to users. Use this project as a learning tool or starting point for more complex CDN simulations.
