/**
 * @file hi_map.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-29
 * 
 * @copyright Copyright 2024 Cosmade
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "hi_map.h"

#define _HI_MAP_BLACK_ITER(__iter__) ((__iter__)|((hi_iter_t)1))
#define _HI_MAP_RED_ITER(__iter__) (HI_MAP_ITER(__iter__))

#define _HI_MAP_NODE_TRUST(__map__, __iter__) ((hi_map_node_t *)hi_mem_pool_get((__map__)->pool, HI_MAP_ITER(__iter__)))

#define _HI_MAP_NODE_LEFT(__map__, __iter__) HI_MAP_ITER(HI_MAP_NODE(__map__, __iter__)->left)
#define _HI_MAP_NODE_RIGHT(__map__, __iter__) HI_MAP_ITER(HI_MAP_NODE(__map__, __iter__)->right)
#define _HI_MAP_NODE_PARENT(__map__, __iter__) HI_MAP_ITER(HI_MAP_NODE(__map__, __iter__)->parent)

#define _HI_MAP_NODE_IS_BLACK(__map__, __iter__) (HI_MAP_NODE_COLOR(__map__, __iter__) == HI_MAP_BLACK)
#define _HI_MAP_NODE_IS_RED(__map__, __iter__) (HI_MAP_NODE_COLOR(__map__, __iter__) == HI_MAP_RED)

#define _HI_MAP_SET_PARENT_BLACK(__map__, __iter__, __parent__) HI_MAP_NODE(__map__, __iter__)->parent = (__parent__ == HI_ITER_NULL?__parent__: _HI_MAP_BLACK_ITER(__parent__))
#define _HI_MAP_SET_PARENT_RED(__map__, __iter__, __parent__) HI_MAP_NODE(__map__, __iter__)->parent = (__parent__ == HI_ITER_NULL?__parent__: _HI_MAP_RED_ITER(__parent__))

#define _HI_MAP_SET_PARENT(__map__, __iter__, __parent__) HI_MAP_NODE(__map__, __iter__)->parent = (HI_MAP_ITER_COLOR(__iter__) == HI_MAP_BLACK ? _HI_MAP_BLACK_ITER(__parent__):_HI_MAP_RED_ITER(__parent__))
#define _HI_MAP_SET_BLACK(__map__, __iter__) HI_MAP_NODE(__map__, __iter__)->parent = (HI_MAP_NODE(__map__, __iter__)->parent != HI_ITER_NULL ? _HI_MAP_BLACK_ITER(HI_MAP_NODE(__map__, __iter__)->parent) : HI_ITER_NULL)
#define _HI_MAP_SET_RED(__map__, __iter__) HI_MAP_NODE(__map__, __iter__)->parent = (HI_MAP_NODE(__map__, __iter__)->parent != HI_ITER_NULL ? _HI_MAP_RED_ITER(HI_MAP_NODE(__map__, __iter__)->parent) : HI_ITER_NULL)


inline hi_map_t* hi_map_new(hi_size_t data_size, hi_size_t max_size)
{
	hi_map_t *map = hi_malloc(sizeof(hi_map_t));
	map->pool = hi_mem_pool_new((hi_mem_pool_config_t){
        .use_check = 1,
        .is_dynamic = 1,
        .block_size = sizeof(hi_map_node_t) + data_size,
        .block_count = max_size
    });
	return map;
}

inline void hi_map_init(hi_map_t *map)
{
    map->root = HI_ITER_NULL;
	map->usage = 0;
	// TODO: found a new way to solve use check.
	// map->pool->config.use_check = 0;
}

inline void hi_map_init_with_pool(hi_map_t *map, hi_mem_pool_t *pool)
{
	map->root = HI_ITER_NULL;
	map->usage = 0;
	map->pool = pool;
}

inline void hi_map_deinit(hi_map_t *map)
{
	hi_map_del_all(map);
	hi_mem_pool_free(map->pool);
	map->pool = NULL;
	map->root = HI_ITER_NULL;
}

inline void hi_map_free(hi_map_t *map)
{
	hi_map_deinit(map);
	hi_free(map);
}

void __hi_map_change_child(hi_map_t *map, hi_iter_t old, hi_iter_t new, hi_iter_t parent)
{
	// change_child
	if (parent != HI_ITER_NULL) {
		if (HI_MAP_ITER(HI_MAP_NODE(map, parent)->left) == HI_MAP_ITER(old))
		{
			HI_MAP_NODE(map, parent)->left = new;
		}
		else
		{
			HI_MAP_NODE(map, parent)->right = new;
		}
	} else {
		map->root = new;
	}
}

void __hi_map_rotate_set_parents(hi_map_t *map, hi_iter_t old, hi_iter_t new, uint8_t color)
{
	hi_iter_t parent = HI_MAP_ITER(HI_MAP_NODE(map, old)->parent);
	HI_MAP_NODE(map, new)->parent = HI_MAP_NODE(map, old)->parent;

	if (color == HI_MAP_BLACK)
	{
		_HI_MAP_SET_PARENT_BLACK(map, old, new);
	}
	else
	{
		_HI_MAP_SET_PARENT_RED(map, old, new);
	}

	__hi_map_change_child(map, old, new, parent);
}

inline hi_iter_t hi_map_set_value(hi_map_t *map, hi_map_key_t key, hi_value_t value)
{
	return hi_map_set(map, key, &value, sizeof(value));
}

hi_iter_t hi_map_set(hi_map_t *map, hi_map_key_t key, const void* data, hi_size_t size)
{
    if (map == NULL) return HI_ITER_NULL;
    if (map->pool == NULL || size > map->pool->config.block_size) return HI_ITER_NULL;
    if (map->root == HI_ITER_NULL)
    {
        map->root = hi_mem_pool_take(map->pool);
        if (map->root == HI_ITER_NULL) return HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->left = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->right = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->parent = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->key = key;

		hi_memcpy(_HI_MAP_NODE_TRUST(map, map->root)->data, data, size);
		map->root = _HI_MAP_BLACK_ITER(map->root);
		map->usage++;
        return HI_MAP_ITER(map->root);
    }

    hi_iter_t node = map->root;
    while (node != HI_ITER_NULL)
    {
        if (key.byte < _HI_MAP_NODE_TRUST(map, node)->key.byte)
        {
            if (_HI_MAP_NODE_TRUST(map, node)->left == HI_ITER_NULL)
            {
                _HI_MAP_NODE_TRUST(map, node)->left = hi_mem_pool_take(map->pool);
                if (_HI_MAP_NODE_TRUST(map, node)->left == HI_ITER_NULL) return HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, _HI_MAP_NODE_TRUST(map, node)->left)->parent = node;

                node = _HI_MAP_NODE_TRUST(map, node)->left;
                _HI_MAP_NODE_TRUST(map, node)->left = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->right = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->key = key;
				hi_memcpy(_HI_MAP_NODE_TRUST(map, node)->data, data, size);
				map->usage++;
                break;
            }
            else
            {
                node = _HI_MAP_NODE_TRUST(map, node)->left;
            }
        }
        else if (key.byte > _HI_MAP_NODE_TRUST(map, node)->key.byte)
        {
            if (_HI_MAP_NODE_TRUST(map, node)->right == HI_ITER_NULL)
            {
                _HI_MAP_NODE_TRUST(map, node)->right = hi_mem_pool_take(map->pool);
                if (_HI_MAP_NODE_TRUST(map, node)->right == HI_ITER_NULL) return HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, _HI_MAP_NODE_TRUST(map, node)->right)->parent = node;

                node = _HI_MAP_NODE_TRUST(map, node)->right;
                _HI_MAP_NODE_TRUST(map, node)->left = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->right = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->key = key;
				hi_memcpy(_HI_MAP_NODE_TRUST(map, node)->data, data, size);
				map->usage++;
                break;
            }
            else
            {
                node = _HI_MAP_NODE_TRUST(map, node)->right;
            }
        }
        else
        {
			hi_memcpy(_HI_MAP_NODE_TRUST(map, node)->data, data, size);
            return HI_MAP_ITER(node);
        }
    }

    //Fix insert. 
    hi_iter_t parent = _HI_MAP_NODE_TRUST(map, node)->parent;
    hi_iter_t gparent = HI_ITER_NULL;
	hi_iter_t tmp = HI_ITER_NULL;
	hi_iter_t result = node;
    while (1) {
		/*
		 * Loop invariant: node is red.
		 */
		if (parent == HI_ITER_NULL) {
			/*
			 * The inserted node is root. Either this is the
			 * first node, or we recursed at Case 1 below and
			 * are no longer violating 4).
			 */
			// node = _HI_MAP_BLACK_ITER(node);
			break;
		}

		/*
		 * If there is a black parent, we are done.
		 * Otherwise, take some corrective action as,
		 * per 4), we don't want a red root or two
		 * consecutive red nodes.
		 */
		if(_HI_MAP_NODE_IS_BLACK(map, parent))
			break;

		gparent = _HI_MAP_NODE_TRUST(map, parent)->parent;
		tmp = HI_MAP_NODE(map, gparent)->right;
		
		if (HI_MAP_ITER(parent) != HI_MAP_ITER(tmp)) {	/* parent == gparent->rb_left */
			if (tmp != HI_ITER_NULL && _HI_MAP_NODE_IS_RED(map, tmp)) {
				/*
				 * Case 1 - node's uncle is red (color flips).
				 *
				 *       G            g
				 *      / \          / \
				 *     p   u  -->   P   U
				 *    /            /
				 *   n            n
				 *
				 * However, since g's parent might be red, and
				 * 4) does not allow this, we need to recurse
				 * at g.
				 */
				_HI_MAP_NODE_TRUST(map, tmp)->parent = _HI_MAP_BLACK_ITER(gparent);
				_HI_MAP_NODE_TRUST(map, parent)->parent = _HI_MAP_BLACK_ITER(gparent);
				node = gparent;
				parent = HI_MAP_ITER(HI_MAP_NODE(map, node)->parent);
				_HI_MAP_SET_PARENT_RED(map, node, parent);
				// printf("case1\n");
				continue;
			}

			tmp = HI_MAP_NODE(map, parent)->right;
			if (HI_MAP_ITER(node) == HI_MAP_ITER(tmp)) {
				/*
				 * Case 2 - node's uncle is black and node is
				 * the parent's right child (left rotate at parent).
				 *
				 *      G             G
				 *     / \           / \
				 *    p   U  -->    n   U
				 *     \           /
				 *      n         p
				 *
				 * This still leaves us in violation of 4), the
				 * continuation into Case 3 will fix that.
				 */
				tmp = HI_MAP_NODE(map, node)->left;
				HI_MAP_NODE(map, parent)->right = tmp;
				HI_MAP_NODE(map, node)->left = parent;
				if (tmp != HI_ITER_NULL) {
					_HI_MAP_SET_PARENT_BLACK(map, tmp, parent);
				}
				_HI_MAP_SET_PARENT_RED(map, parent, node);
				// __hi_map_rotate(map, parent, node);
				parent = node;
				tmp = HI_MAP_NODE(map, node)->right;
				// printf("case2\n");
			}

			/*
			 * Case 3 - node's uncle is black and node is
			 * the parent's left child (right rotate at gparent).
			 *
			 *        G           P
			 *       / \         / \
			 *      p   U  -->  n   g
			 *     /                 \
			 *    n                   U
			 */
			HI_MAP_NODE(map, gparent)->left = tmp;
			/* == parent->rb_right */
			HI_MAP_NODE(map, parent)->right = gparent;
			if (tmp != HI_ITER_NULL)
			{
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
			}
			__hi_map_rotate_set_parents(map, gparent, parent, HI_MAP_RED);
			// __hi_map_rotate(map, gparent, parent);
			// printf("case3\n");
			break;
		} else {
			tmp = HI_MAP_NODE(map, gparent)->left;
			if (tmp != HI_ITER_NULL && _HI_MAP_NODE_IS_RED(map, tmp)) {
				/* Case 1 - color flips */
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
				_HI_MAP_SET_PARENT_BLACK(map, parent, gparent);
				node = gparent;
				parent = HI_MAP_ITER(HI_MAP_NODE(map, node)->parent);
				_HI_MAP_SET_PARENT_RED(map, node, parent);
				// printf("case4\n");
				continue;
			}

			tmp = HI_MAP_NODE(map, parent)->left;
			if (HI_MAP_ITER(node) == HI_MAP_ITER(tmp)) {
				/* Case 2 - right rotate at parent */
				tmp = HI_MAP_NODE(map, node)->right;
				HI_MAP_NODE(map, parent)->left = tmp;
				HI_MAP_NODE(map, node)->right = parent;
				if (tmp != HI_ITER_NULL) 
				{
					_HI_MAP_SET_PARENT_BLACK(map, tmp, parent);
				}
				_HI_MAP_SET_PARENT_RED(map, parent, node);
				// __hi_map_rotate(map, parent, node);
				parent = node;
				tmp = HI_MAP_NODE(map, node)->left;
				// printf("case5\n");
			}

			/* Case 3 - left rotate at gparent */
			HI_MAP_NODE(map, gparent)->right = tmp;
			/* == parent->rb_left */
			HI_MAP_NODE(map, parent)->left = gparent;

			if (tmp != HI_ITER_NULL){
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
				// printf("case6-1\n");
			}
			__hi_map_rotate_set_parents(map, gparent, parent, HI_MAP_RED);
			// __hi_map_rotate(map, gparent, parent);
			// printf("case6\n");
			break;
		}
	}
	if (map->root != HI_ITER_NULL)
	{
		map->root = _HI_MAP_BLACK_ITER(map->root);
	}

    return HI_MAP_ITER(result);
}

inline void* hi_map_get(hi_map_t *map, hi_map_key_t key)
{
	hi_iter_t iter = hi_map_get_iter(map, key);
	if (iter == HI_ITER_NULL) return NULL;
	return _HI_MAP_NODE_TRUST(map, iter)->data;
}

inline hi_map_node_t* hi_map_get_node(hi_map_t *map, hi_iter_t iter)
{
	return HI_MAP_NODE(map, iter);
}

inline hi_iter_t hi_map_get_iter(hi_map_t *map, hi_map_key_t key)
{
    if (map == NULL) return HI_ITER_NULL;
    if (map->pool == NULL) return HI_ITER_NULL;
    hi_iter_t iter = map->root;
    while (iter != HI_ITER_NULL)
    {
        if (key.byte < _HI_MAP_NODE_TRUST(map, iter)->key.byte)
        {
			iter = _HI_MAP_NODE_TRUST(map, iter)->left;
        }
        else if (key.byte > _HI_MAP_NODE_TRUST(map, iter)->key.byte)
        {
			iter = _HI_MAP_NODE_TRUST(map, iter)->right;
        }
        else
        {
            return HI_MAP_ITER(iter);
        }
    }
    return HI_ITER_NULL;
}

hi_iter_t __hi_map_find_left_last(hi_map_t *map, hi_iter_t node)
{
	if (node == HI_ITER_NULL) return node;
	while (_HI_MAP_NODE_LEFT(map, node) != HI_ITER_NULL)
	{
		node = _HI_MAP_NODE_LEFT(map, node);
	}
	return node;
}

hi_iter_t __hi_map_find_right_last(hi_map_t *map, hi_iter_t node)
{
	if (node == HI_ITER_NULL) return node;
	while (_HI_MAP_NODE_RIGHT(map, node) != HI_ITER_NULL)
	{
		node = _HI_MAP_NODE_RIGHT(map, node);
	}
	return node;
}

inline void hi_map_del(hi_map_t *map, hi_map_key_t key)
{
	// struct rb_node *node = NULL, *sibling, *tmp1, *tmp2;
	if (map == NULL || map->root == HI_ITER_NULL || map->usage == 0) return;

	hi_iter_t node = hi_map_get_iter(map, key);
	if (node == HI_ITER_NULL) return;

	hi_iter_t sibling = HI_ITER_NULL;
	hi_iter_t parent = HI_ITER_NULL;
	hi_iter_t tmp1 = HI_MAP_NODE(map, node)->left;
	hi_iter_t tmp2 = HI_MAP_NODE(map, node)->right;

	if (tmp1 == HI_ITER_NULL || tmp2 == HI_ITER_NULL)
	{
		if (tmp2 == HI_ITER_NULL) tmp2 = tmp1;
		tmp1 = HI_ITER_NULL;
		if (HI_MAP_ITER(node) == HI_MAP_ITER(map->root)) 
		{
			hi_mem_pool_bring(map->pool, map->root);
			map->usage--;
			map->root = tmp2;
			if (tmp2 != HI_ITER_NULL) HI_MAP_NODE(map, tmp2)->parent = HI_ITER_NULL;
			return ;
		}
		parent = _HI_MAP_NODE_PARENT(map, node);
		if (HI_MAP_ITER(node) == _HI_MAP_NODE_LEFT(map, parent))
		{
			HI_MAP_NODE(map, parent)->left = tmp2;
		}
		else
		{
			HI_MAP_NODE(map, parent)->right = tmp2;
		}
		hi_mem_pool_bring(map->pool, HI_MAP_ITER(node));
		map->usage--;
		if (tmp2 != HI_ITER_NULL)
		{
			HI_MAP_NODE(map, tmp2)->parent = parent;
			_HI_MAP_SET_BLACK(map, tmp2);
		}
		node = tmp2;
		tmp2 = HI_ITER_NULL;
		return;
	}
	else
	{
		parent = _HI_MAP_NODE_PARENT(map, node);
		if (HI_MAP_ITER(node) == _HI_MAP_NODE_LEFT(map, parent))
		{
			//The left biggest.
			sibling = __hi_map_find_right_last(map, tmp1);
			HI_MAP_NODE(map, sibling)->right = _HI_MAP_NODE_LEFT(map, node);
			if (HI_MAP_ITER(sibling) == HI_MAP_ITER(tmp1))
			{
				HI_MAP_NODE(map, sibling)->left = HI_ITER_NULL;
			}
			else
			{
				HI_MAP_NODE(map, sibling)->left = _HI_MAP_NODE_LEFT(map, node);
			}
			HI_MAP_NODE(map, sibling)->parent = parent;
			HI_MAP_NODE(map, parent)->left = sibling;
			_HI_MAP_SET_BLACK(map, sibling);
		}
		else
		{	
			//The right minimal.
			sibling = __hi_map_find_left_last(map, tmp2);
			HI_MAP_NODE(map, sibling)->left = _HI_MAP_NODE_LEFT(map, node);
			if (HI_MAP_ITER(sibling) == HI_MAP_ITER(tmp2))
			{
				HI_MAP_NODE(map, sibling)->right = HI_ITER_NULL;
			}
			else
			{
				HI_MAP_NODE(map, sibling)->right = _HI_MAP_NODE_RIGHT(map, node);
			}
			HI_MAP_NODE(map, sibling)->parent = parent;
			HI_MAP_NODE(map, parent)->right = sibling;
			_HI_MAP_SET_BLACK(map, sibling);
		}
		hi_mem_pool_bring(map->pool, node);
		map->usage--;
		node = sibling;
	}
	sibling = HI_ITER_NULL;
	tmp1 = HI_ITER_NULL;
	tmp2 = HI_ITER_NULL;

	while (1) {
		/*
		 * Loop invariants:
		 * - node is black (or NULL on first iteration)
		 * - node is not the root (parent is not NULL)
		 * - All leaf paths going through parent and node have a
		 *   black node count that is 1 lower than other leaf paths.
		 */
		sibling = HI_MAP_NODE(map, parent)->right;
		if (HI_MAP_ITER(node) != HI_MAP_ITER(sibling)) {	/* node == parent->rb_left */
			if (_HI_MAP_NODE_IS_RED(map, sibling)) {
				/*
				 * Case 1 - left rotate at parent
				 *
				 *     P               S
				 *    / \             / \
				 *   N   s    -->    p   Sr
				 *      / \         / \
				 *     Sl  Sr      N   Sl
				 */
				tmp1 = HI_MAP_NODE(map, sibling)->left;
				HI_MAP_NODE(map, parent)->right = tmp1;
				HI_MAP_NODE(map, sibling)->left = parent;
				_HI_MAP_SET_PARENT_BLACK(map, tmp1, parent);
				__hi_map_rotate_set_parents(map, parent, sibling, HI_MAP_RED);
				sibling = tmp1;
			}
			
			tmp1 = HI_MAP_NODE(map, sibling)->right;
			if (tmp1 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp1)) {
				tmp2 = HI_MAP_NODE(map, sibling)->left;
				if (tmp2 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp2)) {
					/*
					 * Case 2 - sibling color flip
					 * (p could be either color here)
					 *
					 *    (p)           (p)
					 *    / \           / \
					 *   N   S    -->  N   s
					 *      / \           / \
					 *     Sl  Sr        Sl  Sr
					 *
					 * This leaves us violating 5) which
					 * can be fixed by flipping p to black
					 * if it was red, or by recursing at p.
					 * p is red when coming from Case 1.
					 */
					_HI_MAP_SET_PARENT_RED(map, sibling, parent);
					if (_HI_MAP_NODE_IS_RED(map, parent)){
						_HI_MAP_SET_BLACK(map, parent);
					}
					else {
						node = parent;
						parent = _HI_MAP_NODE_PARENT(map, node);
						if (parent != HI_ITER_NULL){
							continue;
						}
					}
					break;
				}
				/*
				 * Case 3 - right rotate at sibling
				 * (p could be either color here)
				 *
				 *   (p)           (p)
				 *   / \           / \
				 *  N   S    -->  N   sl
				 *     / \             \
				 *    sl  Sr            S
				 *                       \
				 *                        Sr
				 *
				 * Note: p might be red, and then both
				 * p and sl are red after rotation(which
				 * breaks property 4). This is fixed in
				 * Case 4 (in __rb_rotate_set_parents()
				 *         which set sl the color of p
				 *         and set p RB_BLACK)
				 *
				 *   (p)            (sl)
				 *   / \            /  \
				 *  N   sl   -->   P    S
				 *       \        /      \
				 *        S      N        Sr
				 *         \
				 *          Sr
				 */
				
				tmp1 = HI_MAP_NODE(map, tmp2)->right;
				HI_MAP_NODE(map, sibling)->left = tmp1;
				HI_MAP_NODE(map, tmp2)->right = sibling;
				HI_MAP_NODE(map, parent)->right = tmp2;
				if (tmp1 != HI_ITER_NULL){
					_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
				}
				tmp1 = sibling;
				sibling = tmp2;
			}
			/*
			 * Case 4 - left rotate at parent + color flips
			 * (p and sl could be either color here.
			 *  After rotation, p becomes black, s acquires
			 *  p's color, and sl keeps its color)
			 *
			 *      (p)             (s)
			 *      / \             / \
			 *     N   S     -->   P   Sr
			 *        / \         / \
			 *      (sl) sr      N  (sl)
			 */
			tmp2 = HI_MAP_NODE(map, sibling)->left;
			HI_MAP_NODE(map, parent)->right = tmp2;
			HI_MAP_NODE(map, sibling)->left = parent;
			_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
			if (tmp2 != HI_ITER_NULL) {
				_HI_MAP_SET_PARENT(map, tmp2, parent);
			}
			__hi_map_rotate_set_parents(map, parent, sibling, HI_MAP_BLACK);
			break;
		} else {
			sibling = HI_MAP_NODE(map, parent)->left;
			if (_HI_MAP_NODE_IS_RED(map, sibling)) {
				/* Case 1 - right rotate at parent */
				tmp1 = HI_MAP_NODE(map, sibling)->right;
				HI_MAP_NODE(map, parent)->left = tmp1;
				HI_MAP_NODE(map, sibling)->right = parent;
				_HI_MAP_SET_PARENT_BLACK(map, tmp1, parent);
				__hi_map_rotate_set_parents(map, parent, sibling, HI_MAP_RED);
				sibling = tmp1;
			}
			tmp1 = HI_MAP_NODE(map, sibling)->left;
			if (tmp1 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp1)) {
				tmp2 = HI_MAP_NODE(map, sibling)->right;
				if (tmp2 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp2)) {
					/* Case 2 - sibling color flip */
					_HI_MAP_SET_PARENT_RED(map, sibling, parent);
					if (_HI_MAP_NODE_IS_RED(map, parent)) {
						_HI_MAP_SET_BLACK(map, parent);
					}
					else {
						node = parent;
						parent = _HI_MAP_NODE_PARENT(map, node);
						if (parent != HI_ITER_NULL) {
							continue;
						}
					}
					break;
				}
				/* Case 3 - left rotate at sibling */
				tmp1 = HI_MAP_NODE(map, tmp2)->left;
				HI_MAP_NODE(map, sibling)->right = tmp1;
				HI_MAP_NODE(map, tmp2)->left = sibling;
				HI_MAP_NODE(map, parent)->left = tmp2;
				if (tmp1 != HI_ITER_NULL) {
					_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
				}
				tmp1 = sibling;
				sibling = tmp2;
			}
			/* Case 4 - right rotate at parent + color flips */
			tmp2 = HI_MAP_NODE(map, sibling)->right;
			HI_MAP_NODE(map, parent)->left = tmp2;
			HI_MAP_NODE(map, sibling)->right = parent;
			_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
			if (tmp2 != HI_ITER_NULL) {
				_HI_MAP_SET_PARENT(map, tmp2, parent);
			}
			__hi_map_rotate_set_parents(map, parent, sibling, HI_MAP_BLACK);
			break;
		}
	}
}

inline void hi_map_del_all(hi_map_t *map)
{
	map->root = HI_ITER_NULL;
	map->usage = 0;
	hi_mem_pool_bring_all(map->pool);
}

inline hi_iter_t hi_map_begin(hi_map_t *map)
{
	return __hi_map_find_left_last(map, map->root);
}

inline hi_iter_t hi_map_next(hi_map_t *map, hi_iter_t node)
{
    if (node == HI_ITER_NULL)
	{
        return HI_ITER_NULL;
	}
  
    /* If we have a right-hand child, go down and then left as far 
       as we can. */  
    if (_HI_MAP_NODE_RIGHT(map, node) != HI_ITER_NULL) {
        node = _HI_MAP_NODE_RIGHT(map, node);
        while (_HI_MAP_NODE_LEFT(map, node) != HI_ITER_NULL)
		{
            node = _HI_MAP_NODE_LEFT(map, node);
		}
        return node;
    }  
  
    /* No right-hand children.  Everything down and left is 
       smaller than us, so any 'next' node must be in the general 
       direction of our parent. Go up the tree; any time the 
       ancestor is a right-hand child of its parent, keep going 
       up. First time it's a left-hand child of its parent, said 
       parent is our 'next' node. */  

	hi_iter_t parent = _HI_MAP_NODE_PARENT(map, node);
    while (parent != HI_ITER_NULL && (HI_MAP_ITER(node) == _HI_MAP_NODE_RIGHT(map, parent)))
	{
        node = parent;
		parent = _HI_MAP_NODE_PARENT(map, node);
	}
  
    return parent;  
}

void __hi_map_dfs(hi_map_t *map, hi_iter_t iter, hi_map_foreach_f func)
{
	if (iter == HI_ITER_NULL) return;

	if (func(map, HI_MAP_NODE(map, iter)->key, HI_MAP_NODE(map, iter)->data).res != HI_RESULT_OK)
	{
		return;
	}
	__hi_map_dfs(map, HI_MAP_NODE(map, iter)->left, func);
	__hi_map_dfs(map, HI_MAP_NODE(map, iter)->right, func);
}

inline void hi_map_foreach(hi_map_t *map, hi_map_foreach_f func)
{
	if (map == NULL) return;
	__hi_map_dfs(map, map->root, func);
}

hi_size_t __hi_map_max_depth(hi_map_t *map, hi_iter_t node) {
    if (node == HI_ITER_NULL) {
        return 0;
    }

	hi_size_t left_depth = __hi_map_max_depth(map, HI_MAP_NODE(map, node)->left);
	hi_size_t right_depth = __hi_map_max_depth(map, HI_MAP_NODE(map, node)->right);

    return (left_depth > right_depth ? left_depth : right_depth) + 1;
}

inline hi_size_t hi_map_depth(hi_map_t *map)
{
	return __hi_map_max_depth(map, map->root);
}

/************************************************************* Sync Map ******************************************************************/

inline hi_sync_map_t* hi_sync_map_new(hi_size_t data_size, hi_size_t max_size)
{
	hi_sync_map_t *map = hi_malloc(sizeof(hi_sync_map_t));
	map->unsafe.pool = hi_mem_pool_new((hi_mem_pool_config_t){
        .use_check = 1,
        .is_dynamic = 1,
        .block_size = sizeof(hi_map_node_t) + data_size,
        .block_count = max_size
    });
	return map;
}

inline void hi_sync_map_init(hi_sync_map_t *map)
{
	hi_mutex_init(&(map->mutex));
	hi_mutex_lock(&(map->mutex));
	hi_map_init(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
}

inline void hi_sync_map_init_with_pool(hi_sync_map_t *map, hi_mem_pool_t *pool)
{
	hi_mutex_init(&(map->mutex));
	hi_mutex_lock(&(map->mutex));
	hi_map_init_with_pool(&(map->unsafe), pool);
	hi_mutex_unlock(&(map->mutex));
}

inline void hi_sync_map_deinit(hi_sync_map_t *map)
{
	hi_mutex_lock(&(map->mutex));
	hi_map_deinit(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
	hi_mutex_deinit(&(map->mutex));
}

inline void hi_sync_map_free(hi_sync_map_t *map)
{
	hi_sync_map_deinit(map);
	hi_free(map);
}

hi_iter_t hi_sync_map_set(hi_sync_map_t *map, hi_map_key_t key, const void* data, hi_size_t size)
{
	hi_mutex_lock(&(map->mutex));
	size = hi_map_set(&(map->unsafe), key, data, size);
	hi_mutex_unlock(&(map->mutex));
	return size;
}

hi_iter_t hi_sync_map_set_value(hi_sync_map_t *map, hi_map_key_t key, hi_value_t value)
{
	hi_mutex_lock(&(map->mutex));
	value.iter = hi_map_set_value(&(map->unsafe), key, value);
	hi_mutex_unlock(&(map->mutex));
	return value.iter;
}

void* hi_sync_map_get(hi_sync_map_t *map, hi_map_key_t key)
{
	void *data = NULL;
	hi_mutex_lock(&(map->mutex));
	data = hi_map_get(&(map->unsafe), key);
	hi_mutex_unlock(&(map->mutex));
	return data;
}

hi_iter_t hi_sync_map_get_copy(hi_sync_map_t *map, hi_map_key_t key, void* data, hi_size_t size)
{
	hi_iter_t iter;
	hi_mutex_lock(&(map->mutex));
	iter = hi_map_get_iter(&(map->unsafe), key);
	if (iter != HI_ITER_NULL)
	{
		hi_memcpy(data, hi_map_get_node(map, iter)->data, size);
	}
	hi_mutex_unlock(&(map->mutex));
	return iter;
}

hi_iter_t hi_sync_get_iter(hi_sync_map_t *map, hi_map_key_t key)
{
	hi_iter_t iter;
	hi_mutex_lock(&(map->mutex));
	iter = hi_map_get_iter(&(map->unsafe), key);
	hi_mutex_unlock(&(map->mutex));
	return iter;
}

extern hi_size_t hi_sync_map_get_usage(hi_sync_map_t *map)
{
	hi_size_t usage;
	hi_mutex_lock(&(map->mutex));
	usage = map->unsafe.usage;
	hi_mutex_unlock(&(map->mutex));
	return usage;
}

hi_map_node_t* hi_sync_map_get_node(hi_sync_map_t *map, hi_iter_t iter)
{
	hi_map_node_t *node;
	hi_mutex_lock(&(map->mutex));
	node = hi_map_get_node(&(map->unsafe), iter);
	hi_mutex_unlock(&(map->mutex));
	return node;
}

void hi_sync_map_del(hi_sync_map_t *map, hi_map_key_t key)
{
	hi_mutex_lock(&(map->mutex));
	hi_map_del(&(map->unsafe), key);
	hi_mutex_unlock(&(map->mutex));
}

void hi_sync_map_del_all(hi_sync_map_t *map)
{
	hi_mutex_lock(&(map->mutex));
	hi_map_del_all(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
}

hi_size_t hi_sync_map_depth(hi_sync_map_t *map)
{
	hi_size_t depth = 0;
	hi_mutex_lock(&(map->mutex));
	depth = hi_map_depth(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
	return depth;
}
