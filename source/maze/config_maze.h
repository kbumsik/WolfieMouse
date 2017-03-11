/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_MAZE_H
#define __CONFIG_MAZE_H

#define CONFIG_MAX_ROW_SIZE	16
#define CONFIG_MAX_COL_SIZE	16
#define CONFIG_QUEUE_MAX_BUFFER (CONFIG_MAX_COL_SIZE*CONFIG_MAX_ROW_SIZE)
#define CONFIG_DEFAULT_MAZE_START		Position{15,0}
#define CONFIG_DEFAULT_MAZE_GOAL		Position{7,7}
#define CONFIG_DEFAULT_START_DIRECTION (row_minus)

#define CONFIG_SET_DEFAULT_FIRST_CELL() do{ \
			setWall(CONFIG_DEFAULT_MAZE_START, row_plus, wall); \
    		setWall(CONFIG_DEFAULT_MAZE_START, col_plus, wall); \
    		setWall(CONFIG_DEFAULT_MAZE_START, row_minus, empty); \
    		setWall(CONFIG_DEFAULT_MAZE_START, col_minus, wall); \
    		} while(0)

#define CONFIG_SET_DEFAULT_GOAL_CELLS() do{ \
			setWall(Position{7,7}, row_plus, empty); \
			setWall(Position{7,7}, col_plus, empty); \
			setWall(Position{8,8}, row_minus, empty); \
			setWall(Position{8,8}, col_minus, empty); \
    		} while(0)

#endif /* __CONFIG_MAZE_H */
