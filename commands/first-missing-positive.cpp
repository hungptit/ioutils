
/**
 * Link: https://leetcode.com/problems/first-missing-positive/
 * 
 * 1. Iterator over all element.
 * 2. If the current nums[k] = k + 1 then continue.
 * 3. Otherwise swap num[k] and num[num[k] - 1] until
 *    + The current value is not positive
 *    + num[k] > N
 *    + num[k] == num[num[k] - 1]
 * 
 * Runtime: O(n)
 * Memory: O(n)
 * 
 * Note: If we cannot have extra memory then we will ask if we can mofdify the array.
 */

class Solution {
  public:
    int firstMissingPositive(vector<int> &nums) {
        for (int i = 0; i < nums.size(); ++i) {
            if (i + 1 == nums[i]) continue;
            int x = nums[i];
            while (x >= 1 && x <= nums.size() && x != nums[x - 1]) {
                swap(x, nums[x - 1]);
            }
        }
        for (int i = 0; i < nums.size(); ++i) {
            if (i + 1 != nums[i]) return i + 1;
        }
        return nums.size() + 1;
    }
};
