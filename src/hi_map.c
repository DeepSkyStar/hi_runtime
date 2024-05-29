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

#define _HI_MAP_BLACK (1)
#define _HI_MAP_RED (0)

#define _HI_MAP_ITER_COLOR(__iter__) ((__iter__)&((hi_iter_t)1))
#define _HI_MAP_ITER(__iter__) (__iter__ == HI_ITER_NULL? __iter__:((__iter__)&(~(hi_iter_t)1)))
#define _HI_MAP_BLACK_ITER(__iter__) ((__iter__)|((hi_iter_t)1))
#define _HI_MAP_RED_ITER(__iter__) (_HI_MAP_ITER(__iter__))

#define _HI_MAP_NODE_TRUST(__map__, __iter__) ((hi_map_node_t *)(__map__->pool->container + ((__iter__)&(~(hi_iter_t)1))))
#define _HI_MAP_NODE(__map__, __iter__) ((hi_map_node_t *)(__map__->pool->container + _HI_MAP_ITER(__iter__)))
#define _HI_MAP_NODE_COLOR(__map__, __iter__) ((__iter__) == HI_ITER_NULL? _HI_MAP_BLACK: _HI_MAP_ITER_COLOR(_HI_MAP_NODE(__map__, __iter__)->parent))

#define _HI_MAP_NODE_LEFT(__map__, __iter__) _HI_MAP_ITER(_HI_MAP_NODE(__map__, __iter__)->left)
#define _HI_MAP_NODE_RIGHT(__map__, __iter__) _HI_MAP_ITER(_HI_MAP_NODE(__map__, __iter__)->right)
#define _HI_MAP_NODE_PARENT(__map__, __iter__) _HI_MAP_ITER(_HI_MAP_NODE(__map__, __iter__)->parent)

#define _HI_MAP_NODE_IS_BLACK(__map__, __iter__) (_HI_MAP_NODE_COLOR(__map__, __iter__) == _HI_MAP_BLACK)
#define _HI_MAP_NODE_IS_RED(__map__, __iter__) (_HI_MAP_NODE_COLOR(__map__, __iter__) == _HI_MAP_RED)

#define _HI_MAP_SET_PARENT_BLACK(__map__, __iter__, __parent__) _HI_MAP_NODE(__map__, __iter__)->parent = (__parent__ == HI_ITER_NULL?__parent__: _HI_MAP_BLACK_ITER(__parent__))
#define _HI_MAP_SET_PARENT_RED(__map__, __iter__, __parent__) _HI_MAP_NODE(__map__, __iter__)->parent = (__parent__ == HI_ITER_NULL?__parent__: _HI_MAP_RED_ITER(__parent__))

#define _HI_MAP_SET_PARENT(__map__, __iter__, __parent__) _HI_MAP_NODE(__map__, __iter__)->parent = (_HI_MAP_ITER_COLOR(__iter__) == _HI_MAP_BLACK ? _HI_MAP_BLACK_ITER(__parent__):_HI_MAP_RED_ITER(__parent__))
#define _HI_MAP_SET_BLACK(__map__, __iter__) _HI_MAP_NODE(__map__, __iter__)->parent = (_HI_MAP_NODE(__map__, __iter__)->parent != HI_ITER_NULL ? _HI_MAP_BLACK_ITER(_HI_MAP_NODE(__map__, __iter__)->parent) : HI_ITER_NULL)
#define _HI_MAP_SET_RED(__map__, __iter__) _HI_MAP_NODE(__map__, __iter__)->parent = (_HI_MAP_NODE(__map__, __iter__)->parent != HI_ITER_NULL ? _HI_MAP_RED_ITER(_HI_MAP_NODE(__map__, __iter__)->parent) : HI_ITER_NULL)


inline void hi_map_init(hi_map_t *map)
{
    map->root = HI_ITER_NULL;
}

void __hi_map_change_child(hi_map_t *map, hi_iter_t old, hi_iter_t new, hi_iter_t parent)
{
	// change_child
	if (parent != HI_ITER_NULL) {
		if (_HI_MAP_ITER(_HI_MAP_NODE(map, parent)->left) == _HI_MAP_ITER(old))
		{
			_HI_MAP_NODE(map, parent)->left = new;
		}
		else
		{
			_HI_MAP_NODE(map, parent)->right = new;
		}
	} else {
		map->root = new;
	}
}

void __hi_map_rotate_set_parents(hi_map_t *map, hi_iter_t old, hi_iter_t new, uint8_t color)
{
	hi_iter_t parent = _HI_MAP_ITER(_HI_MAP_NODE(map, old)->parent);
	_HI_MAP_NODE(map, new)->parent = _HI_MAP_NODE(map, old)->parent;

	if (color == _HI_MAP_BLACK)
	{
		_HI_MAP_SET_PARENT_BLACK(map, old, new);
	}
	else
	{
		_HI_MAP_SET_PARENT_RED(map, old, new);
	}

	__hi_map_change_child(map, old, new, parent);
}

hi_result_t hi_map_set(hi_map_t *map, hi_map_key_t key, hi_value_t value)
{
    if (map == NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_NULL_PTR);
    if (map->pool == NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
    if (map->root == HI_ITER_NULL)
    {
        map->root = hi_mem_pool_take(map->pool);
        if (map->root == HI_ITER_NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
        _HI_MAP_NODE_TRUST(map, map->root)->left = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->right = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->parent = HI_ITER_NULL;
        _HI_MAP_NODE_TRUST(map, map->root)->key = key;
        _HI_MAP_NODE_TRUST(map, map->root)->value = value;
		map->root = _HI_MAP_BLACK_ITER(map->root);
        return HI_RESULT_MAKE_OK;
    }

    hi_iter_t node = map->root;
    while (node != HI_ITER_NULL)
    {
        if (key.byte < _HI_MAP_NODE_TRUST(map, node)->key.byte)
        {
            if (_HI_MAP_NODE_TRUST(map, node)->left == HI_ITER_NULL)
            {
                _HI_MAP_NODE_TRUST(map, node)->left = hi_mem_pool_take(map->pool);
                if (_HI_MAP_NODE_TRUST(map, node)->left == HI_ITER_NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
                _HI_MAP_NODE_TRUST(map, _HI_MAP_NODE_TRUST(map, node)->left)->parent = node;

                node = _HI_MAP_NODE_TRUST(map, node)->left;
                _HI_MAP_NODE_TRUST(map, node)->left = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->right = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->key = key;
                _HI_MAP_NODE_TRUST(map, node)->value = value;
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
                if (_HI_MAP_NODE_TRUST(map, node)->right == HI_ITER_NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
                _HI_MAP_NODE_TRUST(map, _HI_MAP_NODE_TRUST(map, node)->right)->parent = node;

                node = _HI_MAP_NODE_TRUST(map, node)->right;
                _HI_MAP_NODE_TRUST(map, node)->left = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->right = HI_ITER_NULL;
                _HI_MAP_NODE_TRUST(map, node)->key = key;
                _HI_MAP_NODE_TRUST(map, node)->value = value;
                break;
            }
            else
            {
                node = _HI_MAP_NODE_TRUST(map, node)->right;
            }
        }
        else
        {
            _HI_MAP_NODE_TRUST(map, node)->value = value;
            return HI_RESULT_MAKE_OK;
        }
    }

    //Fix insert. 
    hi_iter_t parent = _HI_MAP_NODE_TRUST(map, node)->parent;
    hi_iter_t gparent = HI_ITER_NULL;
	hi_iter_t tmp = HI_ITER_NULL;
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
		tmp = _HI_MAP_NODE(map, gparent)->right;
		
		if (_HI_MAP_ITER(parent) != _HI_MAP_ITER(tmp)) {	/* parent == gparent->rb_left */
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
				parent = _HI_MAP_ITER(_HI_MAP_NODE(map, node)->parent);
				_HI_MAP_SET_PARENT_RED(map, node, parent);
				// printf("case1\n");
				continue;
			}

			tmp = _HI_MAP_NODE(map, parent)->right;
			if (_HI_MAP_ITER(node) == _HI_MAP_ITER(tmp)) {
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
				tmp = _HI_MAP_NODE(map, node)->left;
				_HI_MAP_NODE(map, parent)->right = tmp;
				_HI_MAP_NODE(map, node)->left = parent;
				if (tmp != HI_ITER_NULL) {
					_HI_MAP_SET_PARENT_BLACK(map, tmp, parent);
				}
				_HI_MAP_SET_PARENT_RED(map, parent, node);
				// __hi_map_rotate(map, parent, node);
				parent = node;
				tmp = _HI_MAP_NODE(map, node)->right;
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
			_HI_MAP_NODE(map, gparent)->left = tmp;
			/* == parent->rb_right */
			_HI_MAP_NODE(map, parent)->right = gparent;
			if (tmp != HI_ITER_NULL)
			{
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
			}
			__hi_map_rotate_set_parents(map, gparent, parent, _HI_MAP_RED);
			// __hi_map_rotate(map, gparent, parent);
			// printf("case3\n");
			break;
		} else {
			tmp = _HI_MAP_NODE(map, gparent)->left;
			if (tmp != HI_ITER_NULL && _HI_MAP_NODE_IS_RED(map, tmp)) {
				/* Case 1 - color flips */
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
				_HI_MAP_SET_PARENT_BLACK(map, parent, gparent);
				node = gparent;
				parent = _HI_MAP_ITER(_HI_MAP_NODE(map, node)->parent);
				_HI_MAP_SET_PARENT_RED(map, node, parent);
				// printf("case4\n");
				continue;
			}

			tmp = _HI_MAP_NODE(map, parent)->left;
			if (_HI_MAP_ITER(node) == _HI_MAP_ITER(tmp)) {
				/* Case 2 - right rotate at parent */
				tmp = _HI_MAP_NODE(map, node)->right;
				_HI_MAP_NODE(map, parent)->left = tmp;
				_HI_MAP_NODE(map, node)->right = parent;
				if (tmp != HI_ITER_NULL) 
				{
					_HI_MAP_SET_PARENT_BLACK(map, tmp, parent);
				}
				_HI_MAP_SET_PARENT_RED(map, parent, node);
				// __hi_map_rotate(map, parent, node);
				parent = node;
				tmp = _HI_MAP_NODE(map, node)->left;
				// printf("case5\n");
			}

			/* Case 3 - left rotate at gparent */
			_HI_MAP_NODE(map, gparent)->right = tmp;
			/* == parent->rb_left */
			_HI_MAP_NODE(map, parent)->left = gparent;

			if (tmp != HI_ITER_NULL){
				_HI_MAP_SET_PARENT_BLACK(map, tmp, gparent);
				// printf("case6-1\n");
			}
			__hi_map_rotate_set_parents(map, gparent, parent, _HI_MAP_RED);
			// __hi_map_rotate(map, gparent, parent);
			// printf("case6\n");
			break;
		}
	}
	if (map->root != HI_ITER_NULL)
	{
		map->root = _HI_MAP_BLACK_ITER(map->root);
	}

    return HI_RESULT_MAKE_OK;
}

inline hi_value_t hi_map_get(hi_map_t *map, hi_map_key_t key)
{
	hi_iter_t iter = hi_map_get_iter(map, key);
	if (iter == HI_ITER_NULL) return HI_VALUE_NULL;
	return _HI_MAP_NODE_TRUST(map, iter)->value;
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
            return _HI_MAP_ITER(iter);
        }
    }
    return HI_ITER_NULL;
}

inline hi_map_node_t hi_map_get_node(hi_map_t *map, hi_iter_t iter)
{
	return *(_HI_MAP_NODE(map, iter));
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
	if (map == NULL || map->root == HI_ITER_NULL) return;

	hi_iter_t node = hi_map_get_iter(map, key);
	if (node == HI_ITER_NULL) return;

	hi_iter_t sibling = HI_ITER_NULL;
	hi_iter_t parent = HI_ITER_NULL;
	hi_iter_t tmp1 = _HI_MAP_NODE(map, node)->left;
	hi_iter_t tmp2 = _HI_MAP_NODE(map, node)->right;
	hi_iter_t to_delete = node;

	if (tmp1 == HI_ITER_NULL || tmp2 == HI_ITER_NULL)
	{
		if (tmp2 == HI_ITER_NULL) tmp2 = tmp1;
		tmp1 = HI_ITER_NULL;
		if (_HI_MAP_ITER(node) == _HI_MAP_ITER(map->root)) 
		{
			hi_mem_pool_bring(map->pool, map->root);
			map->root = tmp2;
			if (tmp2 != HI_ITER_NULL) _HI_MAP_NODE(map, tmp2)->parent = HI_ITER_NULL;
			return ;
		}
		parent = _HI_MAP_NODE_PARENT(map, node);
		if (_HI_MAP_ITER(node) == _HI_MAP_NODE_LEFT(map, parent))
		{
			_HI_MAP_NODE(map, parent)->left = tmp2;
		}
		else
		{
			_HI_MAP_NODE(map, parent)->right = tmp2;
		}
		hi_mem_pool_bring(map->pool, _HI_MAP_ITER(node));
		if (tmp2 != HI_ITER_NULL)
		{
			_HI_MAP_NODE(map, tmp2)->parent = parent;
			_HI_MAP_SET_BLACK(map, tmp2);
		}
		node = tmp2;
		tmp2 = HI_ITER_NULL;
		return;
	}
	else
	{
		parent = _HI_MAP_NODE_PARENT(map, node);
		if (_HI_MAP_ITER(node) == _HI_MAP_NODE_LEFT(map, parent))
		{
			//The left biggest.
			sibling = __hi_map_find_right_last(map, tmp1);
			_HI_MAP_NODE(map, sibling)->right = _HI_MAP_NODE_LEFT(map, node);
			if (_HI_MAP_ITER(sibling) == _HI_MAP_ITER(tmp1))
			{
				_HI_MAP_NODE(map, sibling)->left = HI_ITER_NULL;
			}
			else
			{
				_HI_MAP_NODE(map, sibling)->left = _HI_MAP_NODE_LEFT(map, node);
			}
			_HI_MAP_NODE(map, sibling)->parent = parent;
			_HI_MAP_NODE(map, parent)->left = sibling;
			_HI_MAP_SET_BLACK(map, sibling);
		}
		else
		{	
			//The right minimal.
			sibling = __hi_map_find_left_last(map, tmp2);
			_HI_MAP_NODE(map, sibling)->left = _HI_MAP_NODE_LEFT(map, node);
			if (_HI_MAP_ITER(sibling) == _HI_MAP_ITER(tmp2))
			{
				_HI_MAP_NODE(map, sibling)->right = HI_ITER_NULL;
			}
			else
			{
				_HI_MAP_NODE(map, sibling)->right = _HI_MAP_NODE_RIGHT(map, node);
			}
			_HI_MAP_NODE(map, sibling)->parent = parent;
			_HI_MAP_NODE(map, parent)->right = sibling;
			_HI_MAP_SET_BLACK(map, sibling);
		}
		hi_mem_pool_bring(map->pool, node);
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
		sibling = _HI_MAP_NODE(map, parent)->right;
		if (_HI_MAP_ITER(node) != _HI_MAP_ITER(sibling)) {	/* node == parent->rb_left */
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
				tmp1 = _HI_MAP_NODE(map, sibling)->left;
				_HI_MAP_NODE(map, parent)->right = tmp1;
				_HI_MAP_NODE(map, sibling)->left = parent;
				_HI_MAP_SET_PARENT_BLACK(map, tmp1, parent);
				__hi_map_rotate_set_parents(map, parent, sibling, _HI_MAP_RED);
				sibling = tmp1;
			}
			
			tmp1 = _HI_MAP_NODE(map, sibling)->right;
			if (tmp1 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp1)) {
				tmp2 = _HI_MAP_NODE(map, sibling)->left;
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
				
				tmp1 = _HI_MAP_NODE(map, tmp2)->right;
				_HI_MAP_NODE(map, sibling)->left = tmp1;
				_HI_MAP_NODE(map, tmp2)->right = sibling;
				_HI_MAP_NODE(map, parent)->right = tmp2;
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
			tmp2 = _HI_MAP_NODE(map, sibling)->left;
			_HI_MAP_NODE(map, parent)->right = tmp2;
			_HI_MAP_NODE(map, sibling)->left = parent;
			_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
			if (tmp2 != HI_ITER_NULL) {
				_HI_MAP_SET_PARENT(map, tmp2, parent);
			}
			__hi_map_rotate_set_parents(map, parent, sibling, _HI_MAP_BLACK);
			break;
		} else {
			sibling = _HI_MAP_NODE(map, parent)->left;
			if (_HI_MAP_NODE_IS_RED(map, sibling)) {
				/* Case 1 - right rotate at parent */
				tmp1 = _HI_MAP_NODE(map, sibling)->right;
				_HI_MAP_NODE(map, parent)->left = tmp1;
				_HI_MAP_NODE(map, sibling)->right = parent;
				_HI_MAP_SET_PARENT_BLACK(map, tmp1, parent);
				__hi_map_rotate_set_parents(map, parent, sibling, _HI_MAP_RED);
				sibling = tmp1;
			}
			tmp1 = _HI_MAP_NODE(map, sibling)->left;
			if (tmp1 == HI_ITER_NULL || _HI_MAP_NODE_IS_BLACK(map, tmp1)) {
				tmp2 = _HI_MAP_NODE(map, sibling)->right;
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
				tmp1 = _HI_MAP_NODE(map, tmp2)->left;
				_HI_MAP_NODE(map, sibling)->right = tmp1;
				_HI_MAP_NODE(map, tmp2)->left = sibling;
				_HI_MAP_NODE(map, parent)->left = tmp2;
				if (tmp1 != HI_ITER_NULL) {
					_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
				}
				tmp1 = sibling;
				sibling = tmp2;
			}
			/* Case 4 - right rotate at parent + color flips */
			tmp2 = _HI_MAP_NODE(map, sibling)->right;
			_HI_MAP_NODE(map, parent)->left = tmp2;
			_HI_MAP_NODE(map, sibling)->right = parent;
			_HI_MAP_SET_PARENT_BLACK(map, tmp1, sibling);
			if (tmp2 != HI_ITER_NULL) {
				_HI_MAP_SET_PARENT(map, tmp2, parent);
			}
			__hi_map_rotate_set_parents(map, parent, sibling, _HI_MAP_BLACK);
			break;
		}
	}
}

inline hi_iter_t hi_map_root(hi_map_t *map)
{
	return _HI_MAP_ITER(map->root);
}

inline hi_iter_t hi_map_left(hi_map_t *map, hi_iter_t node)
{
	if (map == NULL || map->root == HI_ITER_NULL || node == HI_ITER_NULL || map->pool == NULL) return HI_ITER_NULL;
	return _HI_MAP_NODE_LEFT(map, node);
}
extern hi_iter_t hi_map_right(hi_map_t *map, hi_iter_t node)
{
	if (map == NULL || map->root == HI_ITER_NULL || node == HI_ITER_NULL || map->pool == NULL) return HI_ITER_NULL;
	return _HI_MAP_NODE_RIGHT(map, node);
}
extern hi_iter_t hi_map_parent(hi_map_t *map, hi_iter_t node)
{
	if (map == NULL || map->root == HI_ITER_NULL || node == HI_ITER_NULL || map->pool == NULL) return HI_ITER_NULL;
	return _HI_MAP_NODE_PARENT(map, node);
}

void __hi_map_dfs(hi_map_t *map, hi_iter_t iter, hi_map_foreach_f func)
{
	if (iter == HI_ITER_NULL) return;

	if (func(map, _HI_MAP_NODE(map, iter)->key, _HI_MAP_NODE(map, iter)->value).res != HI_RESULT_OK)
	{
		return;
	}
	__hi_map_dfs(map, _HI_MAP_NODE(map, iter)->left, func);
	__hi_map_dfs(map, _HI_MAP_NODE(map, iter)->right, func);
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

	hi_size_t left_depth = __hi_map_max_depth(map, _HI_MAP_NODE(map, node)->left);
	hi_size_t right_depth = __hi_map_max_depth(map, _HI_MAP_NODE(map, node)->right);

    return (left_depth > right_depth ? left_depth : right_depth) + 1;
}

inline hi_size_t hi_map_depth(hi_map_t *map)
{
	return __hi_map_max_depth(map, map->root);
}

inline void hi_map_deinit(hi_map_t *map)
{
	if (map == NULL || map->pool == NULL) return;
	hi_iter_t node = map->root;
	while (1)
	{
		if (_HI_MAP_NODE(map, node)->left != HI_ITER_NULL)
		{ 
			node =  _HI_MAP_NODE_LEFT(map, node);
			continue;
		}
		if (_HI_MAP_NODE(map, node)->right != HI_ITER_NULL)
		{ 
			node = _HI_MAP_NODE_RIGHT(map, node);
			continue;
		}
		break;
	}
	hi_iter_t to_delete = node;
	while (to_delete != HI_ITER_NULL)
	{
		//root node.
		if (_HI_MAP_NODE(map, node)->parent == HI_ITER_NULL) break;

		while (_HI_MAP_NODE(map, node)->left != HI_ITER_NULL)
		{
			node = _HI_MAP_NODE_LEFT(map, node);
		}

		if (_HI_MAP_NODE(map, _HI_MAP_NODE_PARENT(map, node))->right != HI_ITER_NULL)
		{
			node = _HI_MAP_NODE_RIGHT(map, _HI_MAP_NODE_PARENT(map, node));
			continue;
		}
		else
		{
			node = _HI_MAP_NODE_PARENT(map, node);
		}

		hi_mem_pool_bring(map->pool, _HI_MAP_ITER(to_delete));
		to_delete = node;
	}
	hi_mem_pool_bring(map->pool, _HI_MAP_ITER(node));
	map->root = HI_ITER_NULL;
}



void hi_aysnc_map_init(hi_aysnc_map_t *map)
{
	hi_mutex_init(&(map->mutex));
	hi_mutex_lock(&(map->mutex));
	hi_map_init(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
}

hi_result_t hi_aysnc_map_set(hi_aysnc_map_t *map, hi_map_key_t key, hi_value_t value)
{
	hi_result_t result = HI_RESULT_MAKE_OK;
	hi_mutex_lock(&(map->mutex));
	result = hi_map_set(&(map->unsafe), key, value);
	hi_mutex_unlock(&(map->mutex));
	return result;
}

hi_value_t hi_aysnc_map_get(hi_aysnc_map_t *map, hi_map_key_t key)
{
	hi_value_t value = HI_VALUE_NULL;
	hi_mutex_lock(&(map->mutex));
	value = hi_map_get(&(map->unsafe), key);
	hi_mutex_unlock(&(map->mutex));
	return value;
}

void hi_aysnc_map_del(hi_aysnc_map_t *map, hi_map_key_t key)
{
	hi_mutex_lock(&(map->mutex));
	hi_map_del(&(map->unsafe), key);
	hi_mutex_unlock(&(map->mutex));
}

hi_size_t hi_aysnc_map_depth(hi_aysnc_map_t *map)
{
	hi_size_t depth = 0;
	hi_mutex_lock(&(map->mutex));
	depth = hi_map_depth(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
	return depth;
}

void hi_aysnc_map_deinit(hi_aysnc_map_t *map)
{
	hi_mutex_lock(&(map->mutex));
	hi_map_deinit(&(map->unsafe));
	hi_mutex_unlock(&(map->mutex));
	hi_mutex_deinit(&(map->mutex));
}